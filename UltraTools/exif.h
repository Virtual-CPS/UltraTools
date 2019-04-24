/**************************************************************************
  exif.h  -- A simple ISO C++ library to parse basic EXIF
  information from a JPEG file.

  Based on the description of the EXIF file format at:
  -- http://park2.wakwak.com/~tsuruzoh/Computer/Digicams/exif-e.html
  -- http://www.media.mit.edu/pia/Research/deepview/exif.html
  -- http://www.exif.org/Exif2-2.PDF

  Copyright (c) 2010-2015 Mayank Lahiri
  mlahiri@gmail.com
  All rights reserved.

  VERSION HISTORY:
  ================

  2.2: Release December 2014
  --

  2.1: Released July 2013
  -- fixed a bug where JPEGs without an EXIF SubIFD would not be parsed
  -- fixed a bug in parsing GPS coordinate seconds
  -- fixed makefile bug
  -- added two pathological test images from Matt Galloway
  http://www.galloway.me.uk/2012/01/uiimageorientation-exif-orientation-sample-images/
  -- split main parsing routine for easier integration into Firefox

  2.0: Released February 2013
  -- complete rewrite
  -- no new/delete
  -- added GPS support

  1.0: Released 2010
  */

#ifndef __EXIF_H
#define __EXIF_H

#include <string>

// FLIR Spot:10, Area:5, Line:1
#define FLIR_MEASURE_SPOT_MAX	20
#define	FLIR_MEASURE_AREA_MAX	10

namespace UltralToolsExif
{
	//
	// Class responsible for storing and parsing EXIF information from a JPEG blob
	//
	class ExifInfo
	{
	public:
		// Parsing function for an entire JPEG image buffer.
		//
		// PARAM 'data': A pointer to a JPEG image.
		// PARAM 'length': The length of the JPEG image.
		// RETURN:  PARSE_EXIF_SUCCESS (0) on succes with 'result' filled out
		//          error code otherwise, as defined by the PARSE_EXIF_ERROR_* macros
		int parseEXIF(const unsigned char *data, unsigned length);
		int parseEXIF(const std::wstring &data);

		// Parsing function for an EXIF segment. This is used internally by parseEXIF()
		// but can be called for special cases where only the EXIF section is
		// available (i.e., a blob starting with the bytes "Exif\0\0").
		int parseFromEXIFSegment(const unsigned char *buf, unsigned len);

		int parseExifSubIFD(const unsigned char *buf, unsigned len, unsigned nOffset);
		int parseGpsSubIFD(const unsigned char *buf, unsigned len, unsigned nOffset);
		int parseMakerNote(const unsigned char *buf, unsigned len, unsigned nOffset);
		int parseFlirApp1Segment(const unsigned char *buf, unsigned len, unsigned nOffset);
		int parseFlirRawData(const unsigned char *buf, unsigned len, unsigned nOffset);
		int parseFlirCameraInfo(const unsigned char *buf, unsigned len, unsigned nOffset);
		int parseFlirMeasInfo(const unsigned char *buf, unsigned len, unsigned nOffset);

		// Set all data members to default values.
		void clear();

		bool			m_bExportRawData;
		bool			m_bExportTemperature;
		std::wstring	m_sFileName;

		// Data fields filled out by parseEXIF()
		unsigned	m_nExifHeaderStart;
		unsigned	m_nTiffHeaderStart;
		bool		m_bAlignIntel;          // false = Motorola byte alignment, true = Intel
		std::wstring ImageDescription;     // Image description
		std::wstring Make;                 // Camera manufacturer's name
		std::wstring Model;                // Camera model
		unsigned short Orientation;       // Image orientation, start of data corresponds to
											// 0: unspecified in EXIF data
											// 1: upper left of image
											// 3: lower right of image
											// 6: upper right of image
											// 8: lower left of image
											// 9: undefined
		std::wstring Software;             // Software used
		std::wstring DateTime;             // File change date and time
		std::wstring DateTimeOriginal;     // Original file date and time (may not exist)
		std::wstring DateTimeDigitized;    // Digitization date and time (may not exist)
		double ExposureTime;              // Exposure time in seconds
		double SubjectDistance;           // Distance to focus point in meters
		double FocalLength;               // Focal length of lens in millimeters
		unsigned ImageWidth;              // Image width reported in EXIF data
		unsigned ImageHeight;             // Image height reported in EXIF data
		struct Geolocation_t {            // GPS information embedded in file
			double Latitude;                  // Image latitude expressed as decimal
			double Longitude;                 // Image longitude expressed as decimal
			double Altitude;                  // Altitude in meters, relative to sea level
			char AltitudeRef;                 // 0 = above sea level, -1 = below sea level
			double DOP;                       // GPS degree of precision (DOP)
			struct Coord_t {
				double degrees;
				double minutes;
				double seconds;
				char direction;
			} LatComponents, LonComponents;   // Latitude, Longitude expressed in deg/min/sec
		} GeoLocation;

		struct FlirInfo_t
		{
			// MakerNotes:
			double ImageTemperatureMax;
			double ImageTemperatureMin;
			//double Emissivity;
			//double CameraTemperatureRangeMax;
			//double CameraTemperatureRangeMin;

			//------------------------------------------------------
			// APP1 Segment:
			std::wstring	CreatorSoftware;

			// RawData
			unsigned short	RawThermalImageWidth;
			unsigned short	RawThermalImageHeight;
			std::wstring	RawThermalImageType;
			unsigned short*	RawThermalImage;

			// CameraInfo
			float			Emissivity;
			float			ObjectDistance;
			float			ReflectedApparentTemperature;
			float			AtmosphericTemperature;
			float			IRWindowTemperature;
			float			IRWindowTransmission;
			float			RelativeHumidity;
			float			PlanckR1;
			float			PlanckB;
			float			PlanckF;
			float			AtmosphericTransAlpha1;
			float			AtmosphericTransAlpha2;
			float			AtmosphericTransBeta1;
			float			AtmosphericTransBeta2;
			float			AtmosphericTransX;
			float			CameraTemperatureRangeMax;
			float			CameraTemperatureRangeMin;
			std::wstring	CameraModel;
			std::wstring	CameraPartNumber;
			std::wstring	CameraSerialNumber;
			std::wstring	CameraSoftware;
			std::wstring	LensModel;
			std::wstring	LensPartNumber;
			std::wstring	LensSerialNumber;
			double			FieldOfView;
			std::wstring	FilterModel;
			std::wstring	FilterPartNumber;
			std::wstring	FilterSerialNumber;
			int				PlanckO;
			float			PlanckR2;
			unsigned		RawValueMedian;
			unsigned		RawValueRange;
			std::wstring	DateTimeOriginal;
			unsigned		FocusStepCount;
			float			FocusDistance;
			float			PaletteColors;
/*				AboveColor AboveColor 169 128 128
				BelowColor BelowColor 49 128 128
				OverflowColor OverflowColor 67 215 98
				UnderflowColor UnderflowColor 40 109 239
				Isotherm1Color Isotherm1Color 99 128 128
				Isotherm2Color Isotherm2Color 92 115 209
				PaletteMethod PaletteMethod 0
				PaletteStretch PaletteStretch 1
				PaletteName PaletteName Iron
				Palette Palette(Binary data 672 bytes, use b
				option to extract)
				Meas1Type Meas1Type Area
				Meas1Params Meas1Params 171 147 264 97
				Meas1Label Meas1Label Ar1
				GPSMapDatum GPSMapDatum WGS84*/
			float			PeakSpectralSensitivity;

			// Measure info
			unsigned short	MeasurementSpotCnt;
			CPoint			MeasurementSpot[FLIR_MEASURE_SPOT_MAX];			// 测温点
			float			MeasurementSpotTemp[FLIR_MEASURE_SPOT_MAX];		// 温度值
			unsigned short	MeasurementAreaCnt;
			CRect			MeasurementArea[FLIR_MEASURE_AREA_MAX];			// 测温矩形框
			float			MeasurementAreaMaxTemp[FLIR_MEASURE_AREA_MAX];	// 最高温度
		} FlirInfo;

		ExifInfo()
		{
			FlirInfo.RawThermalImage = NULL;
			m_bExportRawData = TRUE;
			m_bExportTemperature = TRUE;

			clear();
		}

		virtual ~ExifInfo()
		{
			if (FlirInfo.RawThermalImage != NULL)
			{
				delete[] FlirInfo.RawThermalImage;
				FlirInfo.RawThermalImage = NULL;
			}
		}

		int CalculateImageTemperature( );
		private:
			float CalculateRawDataTemperature(unsigned int rawData);
	};
}

// Parse was successful
#define PARSE_EXIF_SUCCESS                    0
// No JPEG markers found in buffer, possibly invalid JPEG file
#define PARSE_EXIF_ERROR_NO_JPEG              1982
// No EXIF header found in JPEG file.
#define PARSE_EXIF_ERROR_NO_EXIF              1983
// Byte alignment specified in EXIF file was unknown (not Motorola or Intel).
#define PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN    1984
// EXIF header was found, but data was corrupted.
#define PARSE_EXIF_ERROR_CORRUPT              1985
// No FLIR APP1 found in JPEG file.
#define PARSE_EXIF_ERROR_NO_FLIR_APP1         1986

#endif
