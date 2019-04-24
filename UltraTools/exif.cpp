/**************************************************************************
  exif.cpp  -- A simple ISO C++ library to parse basic EXIF
  information from a JPEG file.

  Copyright (c) 2010-2015 Mayank Lahiri
  mlahiri@gmail.com
  All rights reserved (BSD License).
  */
#include "stdafx.h"
#include "exif.h"

#include <algorithm>
#include <cstdint>
#include <stdio.h>
#include <vector>

using std::wstring;
using namespace UltralToolsExif;

/*
%formatNumber = (
	'int8u' = > 1, # BYTE
	'string' = > 2, # ASCII
	'int16u' = > 3, # SHORT
	'int32u' = > 4, # LONG
	'rational64u' = > 5, # RATIONAL
	'int8s' = > 6, # SBYTE
	'undef' = > 7, # UNDEFINED
	'binary' = > 7, # (same as undef)
	'int16s' = > 8, # SSHORT
	'int32s' = > 9, # SLONG
	'rational64s' = > 10, # SRATIONAL
	'float' = > 11, # FLOAT
	'double' = > 12, # DOUBLE
	'ifd' = > 13, # IFD(with int32u format)
	'unicode' = > 14, # UNICODE[see Note below]
	'complex' = > 15, # COMPLEX[see Note below]
	'int64u' = > 16, # LONG8[BigTIFF]
	'int64s' = > 17, # SLONG8[BigTIFF]
	'ifd64' = > 18, # IFD8(with int64u format)[BigTIFF]
	# Note: unicode and complex types are not yet properly supported by ExifTool.
	# These are types which have been observed in the Adobe DNG SDK code, but
	# aren't fully supported there either.  We know the sizes, but that's about it.
	# We don't know if the unicode is null terminated, or the format for complex
# (although I suspect it would be two 4-byte floats, real and imaginary).
	);
*/

namespace {

	struct Rational
	{
		uint32_t numerator, denominator;
		operator double() const
		{
			if (denominator < 1e-20)
			{
				return 0;
			}
			return static_cast<double>(numerator) / static_cast<double>(denominator);
		}
	};

	// IF Entry
	class IFEntry
	{
	public:
		using byte_vector = std::vector<uint8_t>;
		using ascii_vector = std::wstring;
		using short_vector = std::vector<uint16_t>;
		using long_vector = std::vector<uint32_t>;
		using rational_vector = std::vector<Rational>;

		IFEntry()
			: tag_(0xFF), format_(0xFF), data_(0), length_(0), val_byte_(nullptr) {}
		IFEntry(const IFEntry &) = delete;
		IFEntry &operator=(const IFEntry &) = delete;
		IFEntry(IFEntry &&other)
			: tag_(other.tag_),
			format_(other.format_),
			data_(other.data_),
			length_(other.length_),
			val_byte_(other.val_byte_)
		{
			other.tag_ = 0xFF;
			other.format_ = 0xFF;
			other.data_ = 0;
			other.length_ = 0;
			other.val_byte_ = nullptr;
		}
		~IFEntry() { delete_union(); }
		unsigned short tag() const { return tag_; }
		void tag(unsigned short tag) { tag_ = tag; }
		unsigned short format() const { return format_; }
		bool format(unsigned short format)
		{
			switch (format)
			{
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x07:
			case 0x09:
			case 0x0a:
			case 0xff:
				break;
			default:
				return false;
			}
			delete_union();
			format_ = format;
			new_union();
			return true;
		}
		unsigned data() const { return data_; }
		void data(unsigned data) { data_ = data; }
		unsigned length() const { return length_; }
		void length(unsigned length) { length_ = length; }

		// functions to access the data
		//
		// !! it's CALLER responsibility to check that format !!
		// !! is correct before accessing it's field          !!
		//
		// - getters are use here to allow future addition
		//   of checks if format is correct
		byte_vector &val_byte() { return *val_byte_; }
		ascii_vector &val_string() { return *val_string_; }
		short_vector &val_short() { return *val_short_; }
		long_vector &val_long() { return *val_long_; }
		rational_vector &val_rational() { return *val_rational_; }

	private:
		// Raw fields
		unsigned short tag_;
		unsigned short format_;
		unsigned data_;
		unsigned length_;

		// Parsed fields
		union
		{
			byte_vector *val_byte_;
			ascii_vector *val_string_;
			short_vector *val_short_;
			long_vector *val_long_;
			rational_vector *val_rational_;
		};

		void delete_union()
		{
			switch (format_)
			{
			case 0x1:
				delete val_byte_;
				val_byte_ = nullptr;
				break;
			case 0x2:
				delete val_string_;
				val_string_ = nullptr;
				break;
			case 0x3:
				delete val_short_;
				val_short_ = nullptr;
				break;
			case 0x4:
				delete val_long_;
				val_long_ = nullptr;
				break;
			case 0x5:
				delete val_rational_;
				val_rational_ = nullptr;
				break;
			case 0xff:
				break;
			default:
				// should not get here
				// should I throw an exception or ...?
				break;
			}
		}
		void new_union()
		{
			switch (format_)
			{
			case 0x1:
				val_byte_ = new byte_vector();
				break;
			case 0x2:
				val_string_ = new ascii_vector();
				break;
			case 0x3:
				val_short_ = new short_vector();
				break;
			case 0x4:
				val_long_ = new long_vector();
				break;
			case 0x5:
				val_rational_ = new rational_vector();
				break;
			case 0xff:
				break;
			default:
				// should not get here
				// should I throw an exception or ...?
				break;
			}
		}
	};

	// Helper functions
	template <typename T, bool alignIntel>
	T parse(const unsigned char *buf);

	template <>
	uint8_t parse<uint8_t, false>(const unsigned char *buf) {
		return *buf;
	}

	template <>
	uint8_t parse<uint8_t, true>(const unsigned char *buf) {
		return *buf;
	}

	template <>
	uint16_t parse<uint16_t, false>(const unsigned char *buf) {
		return (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
	}

	template <>
	uint16_t parse<uint16_t, true>(const unsigned char *buf) {
		return (static_cast<uint16_t>(buf[1]) << 8) | buf[0];
	}

	template <>
	uint32_t parse<uint32_t, false>(const unsigned char *buf) {
		return (static_cast<uint32_t>(buf[0]) << 24) |
			(static_cast<uint32_t>(buf[1]) << 16) |
			(static_cast<uint32_t>(buf[2]) << 8) | buf[3];
	}

	template <>
	uint32_t parse<uint32_t, true>(const unsigned char *buf) {
		return (static_cast<uint32_t>(buf[3]) << 24) |
			(static_cast<uint32_t>(buf[2]) << 16) |
			(static_cast<uint32_t>(buf[1]) << 8) | buf[0];
	}

	template <>
	float parse<float, true>(const unsigned char *buf) {
		unsigned nTem = parse<uint32_t, true>(buf);
		// 按IEEE754标准转换为单精度浮点数
		// s 为符号（sign）；e 为指数（exponent）；m 为有效位数（mantissa）
		int
			s = (nTem >> 31) == 0 ? 1 : -1,
			e = (nTem >> 23) & 0xff,
			m = (e == 0) ? (nTem & 0x7fffff) << 1 : (nTem & 0x7fffff) | 0x800000;
		return s * m * (float)pow(2.f, e - 150);
	}

	template <>
	float parse<float, false>(const unsigned char *buf) {
		unsigned nTem = parse<uint32_t, false>(buf);
		// 按IEEE754标准转换为单精度浮点数
		// s 为符号（sign）；e 为指数（exponent）；m 为有效位数（mantissa）
		int
			s = (nTem >> 31) == 0 ? 1 : -1,
			e = (nTem >> 23) & 0xff,
			m = (e == 0) ? (nTem & 0x7fffff) << 1 : (nTem & 0x7fffff) | 0x800000;
		return s * m * (float)pow(2.f, e - 150);
	}

	template <>
	Rational parse<Rational, true>(const unsigned char *buf) {
		Rational r;
		r.numerator = parse<uint32_t, true>(buf);
		r.denominator = parse<uint32_t, true>(buf + 4);
		return r;
	}

	template <>
	Rational parse<Rational, false>(const unsigned char *buf) {
		Rational r;
		r.numerator = parse<uint32_t, false>(buf);
		r.denominator = parse<uint32_t, false>(buf + 4);
		return r;
	}

	/**
	 * Try to read entry.length() values for this entry.
	 *
	 * Returns:
	 *  true  - entry.length() values were read
	 *  false - something went wrong, vec's content was not touched
	 */
	template <typename T, bool alignIntel, typename C>
	bool extract_values(C &container, const unsigned char *buf, const unsigned base,
		const unsigned len, const IFEntry &entry)
	{
		const unsigned char *data;
		uint32_t reversed_data;
		// if data fits into 4 bytes, they are stored directly in
		// the data field in IFEntry
		if (sizeof(T) * entry.length() <= 4)
		{
			if (alignIntel)
			{
				reversed_data = entry.data();
			}
			else
			{
				reversed_data = entry.data();
				// this reversing works, but is ugly
				unsigned char *data = reinterpret_cast<unsigned char *>(&reversed_data);
				unsigned char tmp;
				tmp = data[0];
				data[0] = data[3];
				data[3] = tmp;
				tmp = data[1];
				data[1] = data[2];
				data[2] = tmp;
			}
			data = reinterpret_cast<const unsigned char *>(&(reversed_data));
		}
		else
		{
			data = buf + base + entry.data();
			if (data + sizeof(T) * entry.length() > buf + len)
			{
				return false;
			}
		}
		container.resize(entry.length());
		for (size_t i = 0; i < entry.length(); ++i)
		{
			container[i] = parse<T, alignIntel>(data + sizeof(T) * i);
		}
		return true;
	}

	template <bool alignIntel>
	void parseIFEntryHeader(const unsigned char *buf, unsigned short &tag,
		unsigned short &format, unsigned &length,
		unsigned &data)
	{
		// Each directory entry is composed of:
		// 2 bytes: tag number (data field)
		// 2 bytes: data format/type
		//          1 = BYTE
		//         *2 = ASCII
		//          3 = SHORT
		//          4 = LONG（指针）
		//         *5 = RATIONAL，Two LONGs
		//         *7 = UNDEFINED
		//          9 = SLONG
		//          10 = SRATIONAL
		// 4 bytes: number of components
		// 4 bytes: data value or offset to data value，上述format/type类型号前有*的，此4字节为offset，否则为值
		tag = parse<uint16_t, alignIntel>(buf);
		format = parse<uint16_t, alignIntel>(buf + 2);
		length = parse<uint32_t, alignIntel>(buf + 4);
		data = parse<uint32_t, alignIntel>(buf + 8);
	}

	template <bool alignIntel>
	void parseIFEntryHeader(const unsigned char *buf, IFEntry &result)
	{
		unsigned short tag;
		unsigned short format;
		unsigned length;
		unsigned data;

		parseIFEntryHeader<alignIntel>(buf, tag, format, length, data);

		result.tag(tag);
		result.format(format);
		result.length(length);
		result.data(data);
	}

	template <bool alignIntel>
	IFEntry parseIFEntry_temp(const unsigned char *buf, const unsigned offs,
		const unsigned base, const unsigned len)
	{
		IFEntry result;

		// check if there even is enough data for IFEntry in the buffer
		if (buf + offs + 12 > buf + len)
		{
			result.tag(0xFF);
			return result;
		}

		parseIFEntryHeader<alignIntel>(buf + offs, result);

		// Parse value in specified format
		switch (result.format())
		{
		case 1:
			if (!extract_values<uint8_t, alignIntel>(result.val_byte(), buf, base, len, result))
				result.tag(0xFF);
			break;
		case 2:
			// string is basically sequence of uint8_t (well, according to EXIF even
			// uint7_t, but
			// we don't have that), so just read it as bytes
			if (!extract_values<uint8_t, alignIntel>(result.val_string(), buf, base, len, result))
				result.tag(0xFF);

			// and cut zero byte at the end, since we don't want that in the
			// std::wstring
			if (result.val_string()[result.val_string().length() - 1] == '\0')
				result.val_string().resize(result.val_string().length() - 1);
			break;
		case 3:
			if (!extract_values<uint16_t, alignIntel>(result.val_short(), buf, base, len, result))
				result.tag(0xFF);
			break;
		case 4:
			if (!extract_values<uint32_t, alignIntel>(result.val_long(), buf, base, len, result))
				result.tag(0xFF);
			break;
		case 5:
			if (!extract_values<Rational, alignIntel>(result.val_rational(), buf, base, len, result))
				result.tag(0xFF);
			break;
		case 7:
		case 9:
		case 10:
			break;
		default:
			result.tag(0xFF);
		}
		return result;
	}

	// helper functions for convinience
	template <typename T>
	T parse_value(const unsigned char *buf, bool alignIntel)
	{
		if (alignIntel)
			return parse<T, true>(buf);
		else
			return parse<T, false>(buf);
	}

	void parseIFEntryHeader(const unsigned char *buf, bool alignIntel,
		unsigned short &tag, unsigned short &format,
		unsigned &length, unsigned &data)
	{
		if (alignIntel)
			parseIFEntryHeader<true>(buf, tag, format, length, data);
		else
			parseIFEntryHeader<false>(buf, tag, format, length, data);
	}

	IFEntry parseIFEntry(const unsigned char *buf, const unsigned offs,
		const bool alignIntel, const unsigned base,
		const unsigned len)
	{
		if (alignIntel)
			return std::move(parseIFEntry_temp<true>(buf, offs, base, len));
		else
			return std::move(parseIFEntry_temp<false>(buf, offs, base, len));
	}
}

//
// Locates the EXIF segment and parses it using parseFromEXIFSegment
//
int ExifInfo::parseEXIF(const unsigned char *buf, unsigned len)
{
	clear();

	// Sanity check: all JPEG files start with 0xFFD8.
	if (!buf || len < 4)
		return PARSE_EXIF_ERROR_NO_JPEG;
	if (buf[0] != 0xFF || buf[1] != 0xD8)
		return PARSE_EXIF_ERROR_NO_JPEG;

	// Sanity check: some cameras pad the JPEG image with null bytes at the end.
	// Normally, we should able to find the JPEG end marker 0xFFD9 at the end
	// of the image, but not always. As long as there are null/0xFF bytes at the
	// end of the image buffer, keep decrementing len until an 0xFFD9 is found,
	// or some other bytes are. If the first non-zero/0xFF bytes from the end are
	// not 0xFFD9, then we can be reasonably sure that the buffer is not a JPEG.
	while (len > 2)
	{
		if (buf[len - 1] == 0 || buf[len - 1] == 0xFF)
			len--;
		else
		{
			if (buf[len - 1] != 0xD9 || buf[len - 2] != 0xFF)
				return PARSE_EXIF_ERROR_NO_JPEG;
			else
				break;
		}
	}

	// Scan for EXIF header (bytes 0xFF 0xE1) and do a sanity check by
	// looking for bytes "Exif\0\0". The marker length data is in Motorola
	// byte order, which results in the 'false' parameter to parse16().
	// The marker has to contain at least the TIFF header, otherwise the
	// EXIF data is corrupt. So the minimum length specified here has to be:
	//   2 bytes: section size
	//   6 bytes: "Exif\0\0" string
	//   2 bytes: TIFF header (either "II" or "MM" string)
	//   2 bytes: TIFF magic (short 0x2a00 in Motorola byte order)
	//   4 bytes: Offset to first IFD
	// =========
	//  16 bytes
	unsigned offs = 0;  // current offset into buffer
	for (offs = 0; offs < len - 1; offs++)
		if (buf[offs] == 0xFF && buf[offs + 1] == 0xE1) break;
	if (offs + 4 > len)
		return PARSE_EXIF_ERROR_NO_EXIF;
	offs += 2;
	unsigned short section_length = parse_value<uint16_t>(buf + offs, false);
	if (offs + section_length > len || section_length < 16)
		return PARSE_EXIF_ERROR_CORRUPT;
	offs += 2;

	m_nExifHeaderStart = offs;
	return parseFromEXIFSegment(buf + offs, len - offs);
}

int ExifInfo::parseEXIF(const wstring &data)
{
	return parseEXIF((const unsigned char *)data.data(), data.length());
}

//
// Main parsing function for an EXIF segment.
//
// PARAM: 'buf' start of the EXIF TIFF, which must be the bytes "Exif\0\0".
// PARAM: 'len' length of buffer
//
int ExifInfo::parseFromEXIFSegment(const unsigned char *buf, unsigned len)
{
	unsigned nextSegmentOffset = len;
	unsigned offs = 0;       // current offset into buffer
	if (!buf || len < 6)
		return PARSE_EXIF_ERROR_NO_EXIF;

	if (!std::equal(buf, buf + 6, "Exif\0\0"))
		return PARSE_EXIF_ERROR_NO_EXIF;
	offs += 6;
	//nextSegmentOffset = m_nExifHeaderStart + parse_value<uint16_t>(buf - 2, false);
	nextSegmentOffset = parse_value<uint16_t>(buf - 2, false); // buf points to the TiffHeader

	// Now parsing the TIFF header. The first two bytes are either "II" or
	// "MM" for Intel or Motorola byte alignment. Sanity check by parsing
	// the unsigned short that follows, making sure it equals 0x2a. The
	// last 4 bytes are an offset into the first IFD, which are added to
	// the global offset counter. For this block, we expect the following
	// minimum size:
	//  2 bytes: 'II' or 'MM'
	//  2 bytes: 0x002a
	//  4 bytes: offset to first IDF
	// -----------------------------
	//  8 bytes
	if (offs + 8 > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	m_nTiffHeaderStart = offs;
	if (buf[offs] == 'I' && buf[offs + 1] == 'I')
		m_bAlignIntel = true;
	else
	{
		if (buf[offs] == 'M' && buf[offs + 1] == 'M')
			m_bAlignIntel = false;
		else
			return PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN;
	}

	offs += 2;
	if (0x2a != parse_value<uint16_t>(buf + offs, m_bAlignIntel))
		return PARSE_EXIF_ERROR_CORRUPT;
	offs += 2;
	unsigned first_ifd_offset = parse_value<uint32_t>(buf + offs, m_bAlignIntel);
	offs += first_ifd_offset - 4;
	if (offs >= len)
		return PARSE_EXIF_ERROR_CORRUPT;

	// Now parsing the first Image File Directory (IFD0, for the main image).
	// An IFD consists of a variable number of 12-byte directory entries. The
	// first two bytes of the IFD section contain the number of directory
	// entries in the section. The last 4 bytes of the IFD contain an offset
	// to the next IFD, which means this IFD must contain exactly 6 + 12 * num
	// bytes of data.
	// 每个DirectoryEntry的数据所在位置的偏移，以TiffHeaderStart为参照点
	if (offs + 2 > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	int num_entries = parse_value<uint16_t>(buf + offs, m_bAlignIntel);
	if (offs + 6 + 12 * num_entries > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	offs += 2;				// 第一个DirectoryEntry的入口点
	unsigned exif_sub_ifd_offset = len;
	unsigned gps_sub_ifd_offset = len;
	while (--num_entries >= 0)
	{
		IFEntry result =
			parseIFEntry(buf, offs, m_bAlignIntel, m_nTiffHeaderStart, len);
		offs += 12;			// 用于定位每个Entry，Entry中数据部分若为offset，则真实值在TiffHeaderStart+offset处
		TRACE1("IFD0 TAG=0x%X\n", result.tag());
		switch (result.tag())
		{
		case 0x10E:	// Image description
			if (result.format() == 2) this->ImageDescription = result.val_string();
			break;

		case 0x10F:	// Digicam make
			if (result.format() == 2) this->Make = result.val_string();
			break;

		case 0x110:	// Digicam model
			if (result.format() == 2) this->Model = result.val_string();
			break;

		case 0x112:	// Orientation of image
			if (result.format() == 3)
				this->Orientation = result.val_short().front();
			break;

		case 0x131:	// Software used for image
			if (result.format() == 2) this->Software = result.val_string();
			break;

		case 0x132:	// EXIF/TIFF date/time of image modification
			if (result.format() == 2) this->DateTime = result.val_string();
			break;

		case 0x8769:	// EXIF SubIFD offset
			exif_sub_ifd_offset = m_nTiffHeaderStart + result.data();
			break;

		case 0x8825:	// GPS IFS offset
			gps_sub_ifd_offset = m_nTiffHeaderStart + result.data();
			break;
		}
	}

	parseExifSubIFD(buf, len, exif_sub_ifd_offset);
	//parseGpsSubIFD(buf, len, gps_sub_ifd_offset);
	parseFlirApp1Segment(buf, len, nextSegmentOffset);

	return PARSE_EXIF_SUCCESS;
}

// Jump to the EXIF SubIFD if it exists and parse all the information
// there. Note that it's possible that the EXIF SubIFD doesn't exist.
// The EXIF SubIFD contains most of the interesting information that a
// typical user might want.
int ExifInfo::parseExifSubIFD(const unsigned char *buf, unsigned len, unsigned nOffset)
{
	if (nOffset + 4 > len)
		return PARSE_EXIF_ERROR_CORRUPT;

	unsigned offs = nOffset;
	unsigned maker_note_offset = len;
	int num_entries = parse_value<uint16_t>(buf + offs, m_bAlignIntel);
	if (offs + 6 + 12 * num_entries > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	offs += 2;	// 用于定位每个Entry，Entry的数据部分若为偏移量，则真正的值在相对TiffHeaderStart的偏移量处
	while (--num_entries >= 0)
	{
		IFEntry result =
			parseIFEntry(buf, offs, m_bAlignIntel, m_nTiffHeaderStart, len);
		switch (result.tag())
		{
		case 0x829A:	// Exposure time in seconds
			if (result.format() == 5)
				this->ExposureTime = result.val_rational().front();
			break;

		case 0x9003:	// Original date and time
			if (result.format() == 2)
				this->DateTimeOriginal = result.val_string();
			break;

		case 0x9004:	// Digitization date and time
			if (result.format() == 2)
				this->DateTimeDigitized = result.val_string();
			break;

		case 0x9206:	// Subject distance
			if (result.format() == 5)
				this->SubjectDistance = result.val_rational().front();
			break;

		case 0x920A:	// Focal length
			if (result.format() == 5)
				this->FocalLength = result.val_rational().front();
			break;

		case 0xA002:	// EXIF Image width
			if (result.format() == 4)
				this->ImageWidth = result.val_long().front();
			else if (result.format() == 3)
				this->ImageWidth = result.val_short().front();
			break;

		case 0xA003:	// EXIF Image height
			if (result.format() == 4)
				this->ImageHeight = result.val_long().front();
			else if (result.format() == 3)
				this->ImageHeight = result.val_short().front();
			break;

		case 0x927C:	// MakerNote.
			maker_note_offset = m_nTiffHeaderStart + result.data();
			break;
		}

		offs += 12;
	}

	parseMakerNote(buf, len, maker_note_offset);

	return PARSE_EXIF_SUCCESS;
}

// Jump to the GPS SubIFD if it exists and parse all the information
// there. Note that it's possible that the GPS SubIFD doesn't exist.
int ExifInfo::parseGpsSubIFD(const unsigned char *buf, unsigned len, unsigned nOffset)
{
	if (nOffset + 4 > len)
		return PARSE_EXIF_ERROR_CORRUPT;

	unsigned offs = nOffset;
	int num_entries = parse_value<uint16_t>(buf + offs, m_bAlignIntel);
	if (offs + 6 + 12 * num_entries > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	offs += 2;
	while (--num_entries >= 0)
	{
		unsigned short tag, format;
		unsigned length, data;
		parseIFEntryHeader(buf + offs, m_bAlignIntel, tag, format, length, data);
//		unsigned short nTag = result.tag();
//		TRACE1("TAG=0x%X\n", result.tag());
//		TRACE1("TAG=0x%X\n", tag);
		switch (tag)
		{
		case 1:		// GPS north or south
			this->GeoLocation.LatComponents.direction = *(buf + offs + 8);
			if (this->GeoLocation.LatComponents.direction == 0)
				this->GeoLocation.LatComponents.direction = '?';
			else if ('S' == this->GeoLocation.LatComponents.direction)
				this->GeoLocation.Latitude = -this->GeoLocation.Latitude;
			break;

		case 2:		// GPS latitude
			if (format == 5 && length == 3)
			{
				this->GeoLocation.LatComponents.degrees = parse_value<Rational>(
						buf + data + m_nTiffHeaderStart, m_bAlignIntel);
				this->GeoLocation.LatComponents.minutes = parse_value<Rational>(
						buf + data + m_nTiffHeaderStart + 8, m_bAlignIntel);
				this->GeoLocation.LatComponents.seconds = parse_value<Rational>(
						buf + data + m_nTiffHeaderStart + 16, m_bAlignIntel);
				this->GeoLocation.Latitude =
						this->GeoLocation.LatComponents.degrees +
						this->GeoLocation.LatComponents.minutes / 60 +
						this->GeoLocation.LatComponents.seconds / 3600;
				if ('S' == this->GeoLocation.LatComponents.direction)
					this->GeoLocation.Latitude = -this->GeoLocation.Latitude;
			}
			break;

		case 3:		// GPS east or west
			this->GeoLocation.LonComponents.direction = *(buf + offs + 8);
			if (this->GeoLocation.LonComponents.direction == 0)
				this->GeoLocation.LonComponents.direction = '?';
			else if ('W' == this->GeoLocation.LonComponents.direction)
				this->GeoLocation.Longitude = -this->GeoLocation.Longitude;
			break;

		case 4:		// GPS longitude
			if (format == 5 && length == 3)
			{
				this->GeoLocation.LonComponents.degrees = parse_value<Rational>(
						buf + data + m_nTiffHeaderStart, m_bAlignIntel);
				this->GeoLocation.LonComponents.minutes = parse_value<Rational>(
						buf + data + m_nTiffHeaderStart + 8, m_bAlignIntel);
				this->GeoLocation.LonComponents.seconds = parse_value<Rational>(
						buf + data + m_nTiffHeaderStart + 16, m_bAlignIntel);
				this->GeoLocation.Longitude =
						this->GeoLocation.LonComponents.degrees +
						this->GeoLocation.LonComponents.minutes / 60 +
						this->GeoLocation.LonComponents.seconds / 3600;
				if ('W' == this->GeoLocation.LonComponents.direction)
					this->GeoLocation.Longitude = -this->GeoLocation.Longitude;
			}
			break;

		case 5:		// GPS altitude reference (below or above sea level)
			this->GeoLocation.AltitudeRef = *(buf + offs + 8);
			if (1 == this->GeoLocation.AltitudeRef)
				this->GeoLocation.Altitude = -this->GeoLocation.Altitude;
			break;

		case 6:		// GPS altitude
			if (format == 5)
			{
				this->GeoLocation.Altitude = parse_value<Rational>(
					buf + data + m_nTiffHeaderStart, m_bAlignIntel);
				if (1 == this->GeoLocation.AltitudeRef)
					this->GeoLocation.Altitude = -this->GeoLocation.Altitude;
			}
			break;

		case 11:	// GPS degree of precision (DOP)
			if (format == 5)
			{
				this->GeoLocation.DOP = parse_value<Rational>(
					buf + data + m_nTiffHeaderStart, m_bAlignIntel);
			}
			break;
		}

		offs += 12;
	}

	return PARSE_EXIF_SUCCESS;
}

// Jump to the MakerNote if it exists and parse all the information there.
// 每个Entry的长度与ExifIFD/ExiffSubIFD相同，数据结构也相同：
//        2 bytes: tag
//        2 bytes: data format/type
//        4 bytes: data length
//        8 bytes: value or offset (由data format/type 确定，若是offset，均相对TiffHeaderStart)
int ExifInfo::parseMakerNote(const unsigned char *buf, unsigned len, unsigned nOffset)
{
	if (nOffset + 4 > len)
		return PARSE_EXIF_ERROR_CORRUPT;

	unsigned offs = nOffset;
	int num_entries = parse_value<uint16_t>(buf + offs, m_bAlignIntel);
	if (offs + 6 + 12 * num_entries > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	offs += 2;
	while (--num_entries >= 0)
	{
		unsigned short tag, format;
		unsigned length, data;
		parseIFEntryHeader(buf + offs, m_bAlignIntel, tag, format, length, data);
		//TRACE2("MakerNote TAG=0x%X,		Value=%.2f\n",
		//	tag, (double)parse_value<Rational>(buf + data + m_nTiffHeaderStart, m_bAlignIntel));
		switch (tag)
		{
		// FLIR Tags
		case 1:		// ImageTemperatureMax
			if (format == 5)
			{
				FlirInfo.ImageTemperatureMax = parse_value<Rational>(
					buf + data + m_nTiffHeaderStart, m_bAlignIntel);
			}
			break;

		case 2:		// ImageTemperatureMin
			if (format == 5)
			{
				FlirInfo.ImageTemperatureMin = parse_value<Rational>(
					buf + data + m_nTiffHeaderStart, m_bAlignIntel);
			}
			break;

		case 3:		// Emissivity
			if (format == 5)
			{
				FlirInfo.Emissivity = (float)parse_value<Rational>(
					buf + data + m_nTiffHeaderStart, m_bAlignIntel);
			}
			break;

		case 5:		// CameraTemperatureRangeMax
			if (format == 5)
			{
				FlirInfo.CameraTemperatureRangeMax = (float)parse_value<Rational>(
					buf + data + m_nTiffHeaderStart, m_bAlignIntel);
			}
			break;

		case 6:		// CameraTemperatureRangeMin
			if (format == 5)
			{
				FlirInfo.CameraTemperatureRangeMin = (float)parse_value<Rational>(
					buf + data + m_nTiffHeaderStart, m_bAlignIntel);
			}
			break;
		}

		offs += 12;
	}

	return PARSE_EXIF_SUCCESS;
}

// Jump to the FLIR FFF images or the FLIR APP1 segment of JPEG images
// if it exists and parse all the information there.
int ExifInfo::parseFlirApp1Segment(const unsigned char *buf, unsigned len, unsigned nOffset)
{
	if (nOffset + 4 > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	unsigned offs = nOffset;
	unsigned nNextSegment = parse_value<uint16_t>(buf + offs, false);
	offs += 2;
	if (!std::equal(buf + offs, buf + offs + 5, "FLIR\0"))
		return PARSE_EXIF_ERROR_NO_FLIR_APP1;
	offs += 8;
	if (!std::equal(buf + offs, buf + offs + 4, "FFF\0"))
		return PARSE_EXIF_ERROR_NO_FLIR_APP1;
	offs += 4;

	TCHAR szTem[100];
	memset(szTem, 0, sizeof(szTem));
	for (short i = 0; i < 16; i++)
	{
		if (buf[offs + i] != '\0')
			szTem[i] = buf[offs + i];
		else
		{
			szTem[i] = '\0';
			break;
		}
	}
	FlirInfo.CreatorSoftware = szTem;
	offs += 16;

	bool bAlignIntel = false;	// FLIR APP1 is big-endian, FFF files are little-endian
	unsigned nFileFormatVersion = parse_value<uint32_t>(buf + offs, bAlignIntel);
	if (nFileFormatVersion <100 || nFileFormatVersion >= 200)
		bAlignIntel = true;
	offs += 4;

	unsigned nOffsetToRecordDirectory = parse_value<uint32_t>(buf + offs, bAlignIntel);
	offs += 4;
	unsigned nNumOfEntries = parse_value<uint32_t>(buf + offs, bAlignIntel);	// number of entries in record directory
	if (nOffset + 10 + nOffsetToRecordDirectory + nNumOfEntries * 32 > len)
		return PARSE_EXIF_ERROR_CORRUPT;
	offs = nOffset + 10 + nOffsetToRecordDirectory;	// base on start of "FFF\0"
	unsigned nRawDataSegmentsHeaderToltalLen = 0;
	for (unsigned i = 0; i < nNumOfEntries; i++)
	{
		// FLIR record entry(ref 3) :
		// 0x00 - int16u record type
		// 0x02 - int16u record subtype : RawData 1 = BE, 2 = LE, 3 = PNG; 1 for other record types
		// 0x04 - int32u record version : seen 0x64, 0x66, 0x67, 0x68, 0x6f, 0x104
		// 0x08 - int32u index id = 1
		// 0x0c - int32u record offset from start of FLIR data
		// 0x10 - int32u record length
		// 0x14 - int32u parent = 0 (? )
		// 0x18 - int32u object number = 0 (? )
		// 0x1c - int32u checksum : 0 for no checksum

		unsigned nEntryOffs = offs + i * 32;
		unsigned nRecType = parse_value<uint16_t>(buf + nEntryOffs, bAlignIntel);
		TRACE1("EntryRecType = 0x%X\n", nRecType);
		if (0 == nRecType)
			break;

		//unsigned nRecSubtype = parse_value<uint16_t>(buf + nEntryOffs + 2, bAlignIntel);
		//unsigned nRecordVersion = parse_value<uint32_t>(buf + nEntryOffs + 4, bAlignIntel);
		unsigned nRecPos = parse_value<uint32_t>(buf + nEntryOffs + 0x0C, bAlignIntel);
		unsigned nRecLen = parse_value<uint32_t>(buf + nEntryOffs + 0x10, bAlignIntel);

		if (nOffset + 10 + nRecPos + nRecLen > len)
			return PARSE_EXIF_ERROR_NO_EXIF;

		switch (nRecType)
		{
		case 0x0001:	// RawData
			nRawDataSegmentsHeaderToltalLen = 12 * nRecLen / 65534 -4;
			parseFlirRawData(buf, len, nOffset + 10 + nRecPos);
			break;
/*
		case 0x0005:	// GainDeadData
			break;

		case 0x0006:	// CoarseData
			break;

		case 0x000E:	// EmbeddedImage
			break;
*/
		case 0x0020:	// CameraInfo
			parseFlirCameraInfo(buf, len, nOffset + 10 + nRecPos + nRawDataSegmentsHeaderToltalLen);
			break;

		case 0x0021:	// MeasurementInfo
			parseFlirMeasInfo(buf, len, nOffset + 10 + nRecPos + nRawDataSegmentsHeaderToltalLen);
			break;

		case 0x0022:	// PaletteInfo
			//AfxMessageBox(_T("Hello PaletteInfo"));
			break;

		case 0x0023:	// TextInfo
			//AfxMessageBox(_T("Hello TextInfo"));
			break;

		case 0x0024:	// EmbeddedAudioFile
			//AfxMessageBox(_T("Hello EmbeddedAudioFile"));
			break;
/*
		case 0x0028:	// PaintData
			break;

		case 0x002A:	// PiP
			break;

		case 0x002B:	// GPSInfo
			AfxMessageBox(_T("Hello GPSInfo"));
			break;

		case 0x002C:	// MeterLink
			break;

		case 0x002E:	// ParameterInfo
			break;
*/		}
	}

	return PARSE_EXIF_SUCCESS;
}

// Jump to the FLIR RawData of JPEG images
// if it exists and parse all the information there.
int ExifInfo::parseFlirRawData(const unsigned char *buf, unsigned len, unsigned nOffset)
{
	bool bAlignIntel = true;
	unsigned nDataForCheck = parse_value<uint16_t>(buf + nOffset, bAlignIntel);
	// the value should be 0x0002 if the byte order is correct
	if (nDataForCheck >= 0x0100)
		bAlignIntel = false;

	FlirInfo.RawThermalImageWidth = parse_value<uint16_t>(buf + nOffset + 1*2, bAlignIntel);
	FlirInfo.RawThermalImageHeight = parse_value<uint16_t>(buf + nOffset + 2*2, bAlignIntel);

	// 提取FLIR RawData
	unsigned int nCnt = FlirInfo.RawThermalImageWidth * FlirInfo.RawThermalImageHeight;
	FlirInfo.RawThermalImage = new uint16_t[nCnt];
	unsigned int offs = nOffset + 16 * 2;
	unsigned short nData, nMark;
	nMark = bAlignIntel ? 0xE1FF : 0xFFE1;
	for (unsigned int nIndex = 0; nIndex < nCnt; )
	{
		nData = parse_value<uint16_t>(buf + offs, bAlignIntel);
		if (nMark == nData)
		{
			offs += 12;			// FLIR没用多个连续FF E1占位，故可直接跳过segment头部描述
			continue;
		}

		FlirInfo.RawThermalImage[nIndex] = nData;
		nIndex++;
		offs += 2;
	}

	// 输出提取的RawData，以便用BC与ExifTool导出结果进行核对
	// 本文件尽量不用MFC
/*	if (m_bExportRawData)
	{
		CStdioFile ioFile;
		uint16_t nValue;
		if (ioFile.Open(_T("d://UltraTools.dat"), CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
		{
			for (unsigned int nIndex = 0; nIndex < nCnt; nIndex++)
			{
				nValue = FlirInfo.RawThermalImage[nIndex];
				ioFile.Write(&nValue, sizeof(uint16_t));
			}
			ioFile.Close();
		}
	}
*/
	if (m_bExportRawData)
	{
		FILE* pStream = NULL;
		uint16_t nValue;
		TCHAR szFileName[256];
		memset(szFileName, 0, sizeof(szFileName));

		if (m_sFileName.length() <= 0)
			_stprintf(szFileName, _T("d://UltraTools.dat"));
		else
			_stprintf(szFileName, _T("%s.dat"), m_sFileName.c_str());

		// 保存数据
		pStream = _tfopen(szFileName, _T("wb"));
		if (pStream != NULL)
		{
			for (unsigned int nIndex = 0; nIndex < nCnt; nIndex++)
			{
				nValue = FlirInfo.RawThermalImage[nIndex];
				fwrite(&nValue, sizeof(uint16_t), 1, pStream);
			}
			fclose(pStream);
		}
	}

	return PARSE_EXIF_SUCCESS;
}

// Jump to the FLIR CameraInfo of JPEG images
// if it exists and parse all the information there.
int ExifInfo::parseFlirCameraInfo(const unsigned char *buf, unsigned len, unsigned nOffset)
{
	bool bAlignIntel = true;
	unsigned nDataForCheck = parse_value<uint16_t>(buf + nOffset, bAlignIntel);
	// the value should be 0x0002 if the byte order is correct
	if (nDataForCheck >= 0x0100)
		bAlignIntel = false;

	//unsigned nImageWidth = parse_value<uint16_t>(buf + nOffset + 0x02, bAlignIntel);
	//unsigned nImageHeight = parse_value<uint16_t>(buf + nOffset + 0x04, bAlignIntel);

	float fTem;
	FlirInfo.Emissivity = parse_value<float>(buf + nOffset + 0x20, bAlignIntel);
	FlirInfo.ObjectDistance = parse_value<float>(buf + nOffset + 0x24, bAlignIntel);
	FlirInfo.ReflectedApparentTemperature = parse_value<float>(buf + nOffset + 0x28, bAlignIntel) - 273.15f;
	FlirInfo.AtmosphericTemperature = parse_value<float>(buf + nOffset + 0x2C, bAlignIntel) - 273.15f;
	FlirInfo.IRWindowTemperature = parse_value<float>(buf + nOffset + 0x30, bAlignIntel) - 273.15f;
	FlirInfo.IRWindowTransmission = parse_value<float>(buf + nOffset + 0x34, bAlignIntel);
	fTem = parse_value<float>(buf + nOffset + 0x3C, bAlignIntel);
	FlirInfo.RelativeHumidity = fTem > 1.0 ? fTem / 100.0f : fTem;
	FlirInfo.PlanckR1 = parse_value<float>(buf + nOffset + 0x58, bAlignIntel);
	FlirInfo.PlanckB = parse_value<float>(buf + nOffset + 0x5C, bAlignIntel);
	FlirInfo.PlanckF = parse_value<float>(buf + nOffset + 0x60, bAlignIntel);
	FlirInfo.AtmosphericTransAlpha1 = parse_value<float>(buf + nOffset + 0x070, bAlignIntel);	//value: 0.006569
	FlirInfo.AtmosphericTransAlpha2 = parse_value<float>(buf + nOffset + 0x074, bAlignIntel);	//value: 0.012620
	FlirInfo.AtmosphericTransBeta1 = parse_value<float>(buf + nOffset + 0x078, bAlignIntel);	//value: -0.002276
	FlirInfo.AtmosphericTransBeta2 = parse_value<float>(buf + nOffset + 0x07C, bAlignIntel);	//value: -0.006670
	FlirInfo.AtmosphericTransX = parse_value<float>(buf + nOffset + 0x080, bAlignIntel);		//value: 1.900000
	FlirInfo.CameraTemperatureRangeMax = parse_value<float>(buf + nOffset + 0x90, bAlignIntel) - 273.15f;
	FlirInfo.CameraTemperatureRangeMin = parse_value<float>(buf + nOffset + 0x94, bAlignIntel) - 273.15f;

/*	0xd4 = > { Name = > 'CameraModel', Format = > 'string[32]' },
	0xf4 = > { Name = > 'CameraPartNumber', Format = > 'string[16]' }, #1
	0x104 = > { Name = > 'CameraSerialNumber', Format = > 'string[16]' }, #1
	0x114 = > { Name = > 'CameraSoftware', Format = > 'string[16]' }, #1 / PH(NC)
	0x170 = > { Name = > 'LensModel', Format = > 'string[32]' },
	# note: it seems that FLIR updated their lenses at some point, so lenses with the same
	# name may have different part numbers(eg.the FOL38 is either 1196456 or T197089)
	0x190 = > { Name = > 'LensPartNumber', Format = > 'string[16]' },
	0x1a0 = > { Name = > 'LensSerialNumber', Format = > 'string[16]' },
*/
	FlirInfo.FieldOfView = parse_value<float>(buf + nOffset + 0x1B4, bAlignIntel);
/*	0x1ec = > { Name = > 'FilterModel', Format = > 'string[16]' },
	0x1fc = > { Name = > 'FilterPartNumber', Format = > 'string[32]' },
	0x21c = > { Name = > 'FilterSerialNumber', Format = > 'string[32]' },
*/
	FlirInfo.PlanckO = parse_value<uint32_t>(buf + nOffset + 0x308, bAlignIntel);
	FlirInfo.PlanckR2 = parse_value<float>(buf + nOffset + 0x30C, bAlignIntel);
	FlirInfo.RawValueMedian = parse_value<uint16_t>(buf + nOffset + 0x338, bAlignIntel);	// uint32_t?
	FlirInfo.RawValueRange = parse_value<uint16_t>(buf + nOffset + 0x33C, bAlignIntel);
/*	0x384 = > {
		Name = > 'DateTimeOriginal',
			Description = > 'Date/Time Original',
			Format = > 'undef[10]',
			Groups = > { 2 = > 'Time' },
			RawConv = > q{
			my $tm = Get32u(\$val, 0);
			my $ss = Get32u(\$val, 4) & 0xffff;
			my $tz = Get16s(\$val, 8);
			ConvertUnixTime($tm - $tz * 60).sprintf('.%.3d', $ss).TimeZoneString(-$tz);
		},
			PrintConv = > '$self->ConvertDateTime($val)',
	},
*/
	FlirInfo.FocusStepCount = parse_value<uint16_t>(buf + nOffset + 0x390, bAlignIntel);
	FlirInfo.FocusDistance = parse_value<float>(buf + nOffset + 0x45C, bAlignIntel);

	TRACE1("PlanckR1 = %f\n", FlirInfo.PlanckR1);
	TRACE1("PlanckB = %f\n", FlirInfo.PlanckB);
	TRACE1("PlanckF = %f\n", FlirInfo.PlanckF);
	TRACE1("AtmosphericTransAlpha1 = %f\n", FlirInfo.AtmosphericTransAlpha1);
	TRACE1("AtmosphericTransAlpha2 = %f\n", FlirInfo.AtmosphericTransAlpha2);
	TRACE1("AtmosphericTransBeta1 = %f\n", FlirInfo.AtmosphericTransBeta1);
	TRACE1("AtmosphericTransBeta2 = %f\n", FlirInfo.AtmosphericTransBeta2);
	TRACE1("AtmosphericTransX = %f\n", FlirInfo.AtmosphericTransX);
	TRACE1("CameraTemperatureRangeMax = %f\n", FlirInfo.CameraTemperatureRangeMax);
	TRACE1("CameraTemperatureRangeMin = %f\n", FlirInfo.CameraTemperatureRangeMin);
	TRACE1("PlanckO = %d\n", FlirInfo.PlanckO);
	TRACE1("PlanckR2 = %f\n", FlirInfo.PlanckR2);
	TRACE1("RawValueMedian = %d\n", FlirInfo.RawValueMedian);
	TRACE1("RawValueRange = %d\n", FlirInfo.RawValueRange);
	TRACE1("FocusDistance = %f\n", FlirInfo.FocusDistance);

	return PARSE_EXIF_SUCCESS;
}

// Jump to the FLIR MeasInfo of JPEG images
// if it exists and parse all the information there.
int ExifInfo::parseFlirMeasInfo(const unsigned char *buf, unsigned len, unsigned nOffset)
{
	FlirInfo.MeasurementAreaCnt = 0;
	FlirInfo.MeasurementSpotCnt = 0;

	bool bAlignIntel = true;
	unsigned nDataForCheck = parse_value<uint16_t>(buf + nOffset, bAlignIntel);
	// the value should be 0x0001 if the byte order is correct
	if (nDataForCheck >= 0x0100)
		bAlignIntel = false;

	unsigned offs = nOffset + 12;
	if (offs > len)
		return PARSE_EXIF_ERROR_CORRUPT;

	for (int i = 1; ; i++)
	{
		if (offs + 2 > len)
			break;
		unsigned nRecLen = parse_value<uint16_t>(buf + offs, bAlignIntel);
		if (nRecLen < 0x28 || offs + nRecLen >len)
			break;
	
		// 测温工具坐标点参数长度，点4，矩形8
		// Spot=X,Y; Area=X1,Y1,W,H; Ellipse=XC,YC,X1,Y1,X2,Y2; Line=X1,Y1,X2,Y2
		unsigned nCoordLen = parse_value<uint16_t>(buf + offs + 4, bAlignIntel);

		// nValueOffs之后是温度值？因存储温度而导致一条测温记录的实际长度为nRecLen + nExtendLen？
		unsigned nExtendLen = parse_value<uint16_t>(buf + offs + 6, bAlignIntel);
		unsigned nValueOffs = parse_value<uint16_t>(buf + offs + 8, bAlignIntel);

		// 1:Spot，2:Area，3:Ellipse，4:Line
		// 5:Endpoint，#PH(NC, FLIR Tools v2.0 for Mac generates an empty one of these after each Line)
		// 6:Alarm，#PH seen params : "0 1 0 1 9142 0 9142 0" (called "Isotherm" by Mac version)
		// 7:Unused，#PH(NC) (or maybe "Free" ? )
		// 8:Difference
		unsigned nToolType = parse_value<uint16_t>(buf + offs + 0x0A, bAlignIntel);	// 0x0A = 10
		if (nToolType < 1 || nToolType > 8)
			break;

		unsigned nToolIndex = parse_value<uint16_t>(buf + offs + 0x0C, bAlignIntel);// 0x0C = 12

		// extract measurement parameters
		if (offs + 0x24 + nCoordLen >len)	// 0x24 = 36
			break;
		// Start=>offs+0x24, Size=>nCoordLen
		if (1 == nToolType && 4 == nCoordLen)		// Spot
		{
			long x = parse_value<uint16_t>(buf + offs + 0x24, bAlignIntel);
			long y = parse_value<uint16_t>(buf + offs + 0x24 + 2, bAlignIntel);

			FlirInfo.MeasurementSpot[FlirInfo.MeasurementSpotCnt].x = x;
			FlirInfo.MeasurementSpot[FlirInfo.MeasurementSpotCnt].y = y;
			FlirInfo.MeasurementSpotCnt++;

			if (FlirInfo.MeasurementSpotCnt >= FLIR_MEASURE_SPOT_MAX)
				break;
		}
		else if (2 == nToolType && 8 == nCoordLen)	// Area
		{
			long left = parse_value<uint16_t>(buf + offs + 0x24, bAlignIntel);
			long top = parse_value<uint16_t>(buf + offs + 0x24 + 2, bAlignIntel);
			long width = parse_value<uint16_t>(buf + offs + 0x24 + 4, bAlignIntel);
			long height = parse_value<uint16_t>(buf + offs + 0x24 + 6, bAlignIntel);
			FlirInfo.MeasurementArea[FlirInfo.MeasurementAreaCnt].left = left;
			FlirInfo.MeasurementArea[FlirInfo.MeasurementAreaCnt].top = top;
			FlirInfo.MeasurementArea[FlirInfo.MeasurementAreaCnt].right = left + width;
			FlirInfo.MeasurementArea[FlirInfo.MeasurementAreaCnt].bottom = top + height;
			FlirInfo.MeasurementAreaCnt++;

			if (FlirInfo.MeasurementAreaCnt >= FLIR_MEASURE_AREA_MAX)
				break;
		}

		// extract label (sometimes-null-terminated Unicode)
		// 测温工具标签紧随测温工具坐标参数之后，形如“41 00 72 00 31 00”（Ar1）
/*		my @uni;
		for ($p = 0x24 + $coordLen; $p<$recLen - 1; $p += 2) {
			my $ch = Get16u($dataPt, $p + $pos);
			# FLIR Tools v2.0 for Mac doesn't properly null-terminate these strings,
			# so end the string at any funny character
			last if $ch < 0x20 or $ch > 0x7f;
			push @uni, $ch;
		}
		# convert to the ExifTool character set
		require Image::ExifTool::Charset;
		my $val = Image::ExifTool::Charset::Recompose($et, \@uni);
		$et->HandleTag($tagTablePtr, "${pre}Label", $val,
			DataPt = >$dataPt, DataPos = >$dataPos, Start = >$pos + 0x24 + $coordLen, Size = >2 * scalar(@uni));
*/
		offs = offs + nRecLen;		// for next record
	}

	return PARSE_EXIF_SUCCESS;
}

void ExifInfo::clear()
{
	if (FlirInfo.RawThermalImage != NULL)
	{
		delete[] FlirInfo.RawThermalImage;
		FlirInfo.RawThermalImage = NULL;
	}
	FlirInfo.MeasurementAreaCnt = 0;
	FlirInfo.MeasurementSpotCnt = 0;
	memset(FlirInfo.MeasurementSpotTemp, 0, sizeof(FlirInfo.MeasurementSpotTemp));
	memset(FlirInfo.MeasurementAreaMaxTemp, 0, sizeof(FlirInfo.MeasurementAreaMaxTemp));

	// Strings
	ImageDescription = _T("");
	Make = _T("");
	Model = _T("");
	Software = _T("");
	DateTime = _T("");
	DateTimeOriginal = _T("");
	DateTimeDigitized = _T("");

	// Shorts / unsigned / double
	m_bAlignIntel = false;
	Orientation = 0;

	ExposureTime = 0;
	SubjectDistance = 0;
	FocalLength = 0;
	ImageWidth = 0;
	ImageHeight = 0;

	// Geolocation
	GeoLocation.Latitude = 0;
	GeoLocation.Longitude = 0;
	GeoLocation.Altitude = 0;
	GeoLocation.AltitudeRef = 0;
	GeoLocation.DOP = 0;
	GeoLocation.LatComponents.degrees = 0;
	GeoLocation.LatComponents.minutes = 0;
	GeoLocation.LatComponents.seconds = 0;
	GeoLocation.LatComponents.direction = '?';
	GeoLocation.LonComponents.degrees = 0;
	GeoLocation.LonComponents.minutes = 0;
	GeoLocation.LonComponents.seconds = 0;
	GeoLocation.LonComponents.direction = '?';
}

int ExifInfo::CalculateImageTemperature( )
{
	if (NULL == FlirInfo.RawThermalImage)
		return -1;
	if (FlirInfo.RawThermalImageWidth <= 0 || FlirInfo.RawThermalImageHeight <= 0)
		return -2;

	uint16_t x, y, w, h, rawData;
	float fMax, fValue;
	unsigned int nCnt;

	// 导出整幅热图的温度数据，以便用BC与FLIR Tools导出的温度进行误差分析
	// 本文件尽量不用MFC
/*	if (m_bExportTemperature)
	{
		CStdioFile ioFile;
		CString sTem;
		if (m_sFileName.length() <= 0)
			sTem = _T("d://UltraTools.txt");
		else
			sTem.Format(_T("%s.txt"), m_sFileName.c_str());
		if (ioFile.Open(sTem, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
		{
			nCnt = FlirInfo.RawThermalImageWidth * FlirInfo.RawThermalImageHeight;
			for (unsigned int i = 0; i < nCnt; i++)
			{
				sTem.Format(_T("%0.3f\t"), CalculateRawDataTemperature(FlirInfo.RawThermalImage[i]));
				ioFile.WriteString(sTem);

				if (0 == (i + 1) % FlirInfo.RawThermalImageWidth)
					ioFile.WriteString(_T("\n"));
			}

			ioFile.Close();
		}
	}
*/
	if (m_bExportTemperature)
	{
		FILE* pStream = NULL;
		TCHAR szFileName[256], szValue[10];
		memset(szFileName, 0, sizeof(szFileName));
		memset(szValue, 0, sizeof(szValue));

		if (m_sFileName.length() <= 0)
			_stprintf(szFileName, _T("d://UltraTools.txt"));
		else
			_stprintf(szFileName, _T("%s.txt"), m_sFileName.c_str());

		// 保存数据
		pStream = _tfopen(szFileName, _T("wt"));
		if (pStream != NULL)
		{
			nCnt = FlirInfo.RawThermalImageWidth * FlirInfo.RawThermalImageHeight;
			for (unsigned int i = 0; i < nCnt; i++)
			{
				_stprintf(szValue, _T("%0.3f\t"), CalculateRawDataTemperature(FlirInfo.RawThermalImage[i]));
				_fputts(szValue, pStream);

				if (0 == (i + 1) % FlirInfo.RawThermalImageWidth)
					_fputts(_T("\n"), pStream);
			}
			fclose(pStream);
		}
	}

	nCnt = FlirInfo.MeasurementSpotCnt;
	for (unsigned int i = 0; i < nCnt; i++)
	{
		x = (uint16_t)FlirInfo.MeasurementSpot[i].x;
		y = (uint16_t)FlirInfo.MeasurementSpot[i].y;
		if (x > FlirInfo.RawThermalImageWidth - 1 || y > FlirInfo.RawThermalImageHeight - 1)
			return -3;

		rawData = FlirInfo.RawThermalImage[x + y * FlirInfo.RawThermalImageWidth];
		FlirInfo.MeasurementSpotTemp[i] = CalculateRawDataTemperature(rawData);
		TRACE3("\nx=%8d, y=%8d, temperature=%.3f\n", x, y, FlirInfo.MeasurementSpotTemp[i]);
	}

	nCnt = FlirInfo.MeasurementAreaCnt;
	for (unsigned int i = 0; i < nCnt; i++)
	{
		x = (uint16_t)FlirInfo.MeasurementArea[i].left;
		y = (uint16_t)FlirInfo.MeasurementArea[i].top;
		w = (uint16_t)FlirInfo.MeasurementArea[i].Width();
		h = (uint16_t)FlirInfo.MeasurementArea[i].Height();

		if (x + w > FlirInfo.RawThermalImageWidth || y + h > FlirInfo.RawThermalImageHeight)
			return -4;

		fMax = fValue = -9999.f;
		for (uint16_t row = 0; row < h; row++)
		{
			for (uint16_t col = 0; col < w; col++)
			{
				rawData = FlirInfo.RawThermalImage[(x+col) + (y+row) * FlirInfo.RawThermalImageWidth];
				fValue = CalculateRawDataTemperature(rawData);
				if (fValue > fMax)
					fMax = fValue;
			}
		}

		FlirInfo.MeasurementAreaMaxTemp[i] = fMax;
	}

	return 0;
}

float ExifInfo::CalculateRawDataTemperature(unsigned int rawData)
{
	float T_obj = 0.f;

	// 以下公式，源自[Infrared Thermography：Errors and Uncertainties] p164 & p171
	float tAtmCelsius = FlirInfo.AtmosphericTemperature;
	if (tAtmCelsius > 273.15f)
		tAtmCelsius = tAtmCelsius - 273.15f;				// 确保tAtmCelsius是摄氏温度
	float h2o = (float)(FlirInfo.RelativeHumidity * exp(
		1.5587 + (6.939e-2) * tAtmCelsius - (2.7816e-4)*pow(tAtmCelsius, 2) + (6.8455e-7)*pow(tAtmCelsius,3)));
	// Calculation of transmission of atmosphere
	float tau = FlirInfo.AtmosphericTransX * exp(
			(-1)*sqrt(FlirInfo.ObjectDistance)* (FlirInfo.AtmosphericTransAlpha1 + FlirInfo.AtmosphericTransBeta1*sqrt(h2o))) +
			(1 - FlirInfo.AtmosphericTransX) * exp(
			(-1)*sqrt(FlirInfo.ObjectDistance)*(FlirInfo.AtmosphericTransAlpha2 + FlirInfo.AtmosphericTransBeta2*sqrt(h2o)));
	if (tau <= 0.f)
		tau = 0.000001f;
/*
	// 源自[Infrared Thermography：Errors and Uncertainties] p164 & p171
	float R = FlirInfo.PlanckR1 / FlirInfo.PlanckR2;
	float tAmb = tAtmCelsius + 273.15f;	// 环境温度(Ambient temperature)未知，用环境大气温度，用Plank公式计算辐射，故应是开氏温度
	float k1 = 1 / (FlirInfo.Emissivity * tau);
	// k2是object表面反射环境大气辐射及环境大气直接辐射之和，没考虑漂移，即PlanckO
	// 后面源自[Calibration of FLIR Thermacam SC3000 - Infrared Training Center]的公式考虑了漂移，且把tAmb区分为大气温度和反射温度
	// 两公式的“形式”相同
	float k2 = (((1 - FlirInfo.Emissivity) / FlirInfo.Emissivity) *
		(R / (pow(e, FlirInfo.PlanckB / tAmb) - FlirInfo.PlanckF))) + (((1 - tau) /
		(FlirInfo.Emissivity * tau)) * (R / (pow(e, FlirInfo.PlanckB / tAmb) - FlirInfo.PlanckF)));
	float absPixel = rawData + FlirInfo.PlanckO;
	float objSignal = k1 * absPixel - k2;
	T_obj = FlirInfo.PlanckB / log(R/objSignal + FlirInfo.PlanckF);

	// [Infrared Thermography：Errors and Uncertainties]原文：
//	% Calculation of compensated pixel values
//		absPixel = globalGain.*lFunc + globalOffset;
//	% Calculation of detector signal value
//		k1 = 1. / (emissivity.*tau);
//	    k2 = (((1 - emissivity). / emissivity).*...				// 用温度计算辐射时，不考虑漂移globalOffset？
//		     (R. / (exp(B. / tAmb) - F))) + (((1 - tau). / ...
//		     (emissivity.*tau)).*(R. / (exp(B. / tAmb) - F)));
//	    objSignal = (k1.*absPixel / 2) - k2;
//	% Calculation of temperature values
//		y = B. / log((R. / objSignal) + F);
*/

	//float R_refl = rawData / (FlirInfo.Emissivity * tau) - (2 - FlirInfo.Emissivity - tau) * FlirInfo.AtmosphericTemperature;
	float tReflKelvins = FlirInfo.ReflectedApparentTemperature;
	if (tReflKelvins < 273.15f)
		tReflKelvins = tReflKelvins + 273.15f;						// 确保T_refl是开氏温度
	float R_refl = (float)(FlirInfo.PlanckR1 / (FlirInfo.PlanckR2 * (exp(FlirInfo.PlanckB / tReflKelvins) - FlirInfo.PlanckF)) - FlirInfo.PlanckO);

	// 源自：[enhancement_ extract binary data from FLIR radiometric jpg]
	// 没考虑大气传输衰减（ObjectDistance为零，忽略相对湿度）
	//float R_obj = (rawData - (1.0f - FlirInfo.Emissivity) * R_refl) / FlirInfo.Emissivity;

	// 源自：[The statistical approach to the sensitivity analysis of the ThermaCAM PM595 measurement mode]
	// 只考虑了环境反射，没考虑大气辐射
	//float R_obj = (rawData - (1.0f - FlirInfo.Emissivity * tau) * R_refl) / (FlirInfo.Emissivity * tau);

	// 源自：[Calibration of FLIR Thermacam SC3000 - Infrared Training Center]
	float tAtmKelvins = tAtmCelsius + 273.15f;
	float R_atm = (float)(FlirInfo.PlanckR1 / (FlirInfo.PlanckR2 * (exp(FlirInfo.PlanckB / tAtmKelvins) - FlirInfo.PlanckF)) - FlirInfo.PlanckO);
	float R_obj = ((rawData - (1.0f - tau) * R_atm) / tau - (1.0f - FlirInfo.Emissivity) * R_refl) / FlirInfo.Emissivity;

	T_obj = FlirInfo.PlanckB / log(FlirInfo.PlanckR1 / (FlirInfo.PlanckR2*(R_obj + FlirInfo.PlanckO)) + FlirInfo.PlanckF);

	// Fluke温度计算公式，源自[ReadIs2.m]
	//t = tb / (emissivity*transmission) - (2 - emissivity - transmission)*backgroundtemp

	// 当ObjectDistance为0（此时tau==1，无论相对湿度是多少）、emissivity为1时，上述计算可简化
	// 源自：[enhancement_ extract binary data from FLIR radiometric jpg]
	//T_obj = FlirInfo.PlanckB / log(FlirInfo.PlanckR1 / (FlirInfo.PlanckR2*(rawData + FlirInfo.PlanckO)) + FlirInfo.PlanckF);

	T_obj = T_obj - 273.15f;	// 转换为摄氏温度
	return T_obj;
}