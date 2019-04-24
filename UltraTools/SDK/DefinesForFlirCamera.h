// camera.h

#pragma once

// Event codes
#define EV_DEVICE_READY					1 // Not used
#define EV_CONNECTED					2
#define EV_DISCONNECTED					3
#define EV_CONN_BROKEN					4
#define EV_RECONNECTED					5
#define EV_DISCONNECTING				6
#define EV_AUTOADJUST					7
#define EV_RECALIB_START				8
#define EV_RECALIB_STOP					9
#define EV_LUT_UPDATED					10
#define EV_REC_UPDATED					11 // Recording settings are uppdated
#define EV_IMAGE_CAPTURED				12
#define EV_INIT_COMPLETED				13
#define EV_FRAME_RATE_TABLE_AVAILABLE	14
#define EV_FRAME_RATE_CHANGE_COMPLETED	15
#define EV_RANGE_TABLE_AVAILABLE		16
#define EV_RANGE_CHANGE_COMPLETED		17
#define EV_IMAGE_SIZE_CHANGED			18

// Camera action codes (used in DoCameraAction)
#define CAM_ACTION_RECORDING_START     0
#define CAM_ACTION_RECORDING_STOP      1
#define CAM_ACTION_RECORDING_ENABLE    2
#define CAM_ACTION_RECORDING_DISABLE   3
#define CAM_ACTION_RECORDING_PAUSE     4
#define CAM_ACTION_RECORDING_RESUME    5
#define CAM_ACTION_RECORDING_SNAPSHOT  6
#define CAM_ACTION_RECORDING_SHOWDLG   7
#define CAM_ACTION_INT_IMG_CORR        8  // Calibrate using shutter
#define CAM_ACTION_EXT_IMG_CORR        9  // calibrate using externa surface
#define CAM_ACTION_AUTO_ADJUST        10
#define CAM_ACTION_RECORDING_CLEAR    11  // Clear recording file list
#define CAM_ACTION_AUTO_FOCUS         12
#define CAM_ACTION_SHOW_INFO          13
#define CAM_ACTION_SHOW_DEVSTATUS     14
#define CAM_ACTION_GET_CALIB          15
#define CAM_ACTION_RECORDING_KEYTRIG  16
#define CAM_ACTION_CREATE_RESOURCE    17
#define CAM_ACTION_DELETE_RESOURCE    18
#define CAM_ACTION_SAVE_SETTINGS      19
#define CAM_ACTION_RESTORE_SETTINGS   20

// Recording state (as returned from PROP_REC_STATE)
#define REC_STATE_DISABLED             0
#define REC_STATE_WAITING              1
#define REC_STATE_ACTIVE               2
#define REC_STATE_PAUSED               3

// Device codes (used in Connect) and property PROP_DEVICE
#define DEVICE_PCCARD	    0	// PCCard 16 bit frame grabber device
#define DEVICE_ITEX		    1	// ITEX 16 bit frame grabber device
#define DEVICE_NONE		    2
#define DEVICE_FILE		    2	// File device
#define DEVICE_FIREWIRE_16  3	// Firewire interface 16-bit
#define DEVICE_ETHERNET_8   4	// 8-bit RTP images over Ethernet
#define DEVICE_FIREWIRE_8   5	// Firewire interface 8-bit
#define DEVICE_ETHERNET_16  6	// 16-bit images over Ethernet
#define DEVICE_IRFL			7	// IRFlashLink 16 bit frame grabber
#define DEVICE_IPORT		8	// Pleora remote frame grabber using IP (Gigabit)

// Camera type codes (used in Connect) and property PROP_CAMERA_TYPE
#define CAM_500             0
#define CAM_900             1  
#define CAM_THV1000         2  
#define CAM_SC1000          3
#define CAM_THERMACAM       4
#define CAM_THERMOVISION    5
#define CAM_MERLIN			6
#define CAM_PHOENIX			7
#define CAM_OMEGA			8
#define CAM_CUMULUS			9
#define CAM_BTZ				10
#define CAM_A320S			11
#define CAM_A320R			12

// Camera communication interface codes (used in Connect)
#define CAM_INTF_NONE     0
#define CAM_INTF_SERIAL   1
#define CAM_INTF_TCPIP    2
#define CAM_INTF_FIREWIRE 3
#define CAM_INTF_PLEORA   4 // iPort frame grabber
#define CAM_INTF_AXIS	  5 // AXIS Video Server

// Trig sources
#define TRIG_SRC_EXT		0 // Source is camera or I/O board
#define TRIG_SRC_COM		1 // Source is serial RS-232 port
#define TRIG_SRC_LPT		2 // Source is parallell line printer port
#define TRIG_SRC_NONE		3 // Source is not defined

// Camera file formats
#define CAM_FMT_JPEG		0	// JPEG and non-compressed IR pixel data 
#define CAM_FMT_JPEG_PNG	1	// JPEG and PNG-compressed IR pixel data 
#define CAM_FMT_JPEG_ONLY	2	// JPEG without IR data 
#define CAM_FMT_FFF			3	// Only IR (pixel data not compressed) 
#define CAM_FMT_FFF_PNG		4	// Only IR (pixel data is PNG-compressed) 

// Image correction modes
#define IMAGE_CORR_DISABLED		0 // Image correction disabled
#define IMAGE_CORR_EMISSIVITY	1 // Use image emissivity correction map
#define IMAGE_CORR_DISTANCE		2 // Use image distance correction map

// Camera property codes
enum { 
	PROP_TRFL = 0,				// Reflected Temperature (in Kelvin)
	PROP_TATM = 1,				// Atmospheric Temperature (in Kelvin)
	PROP_DOBJ = 2,				// Object Distance (in Meter)
    PROP_EMIS = 3,				// Object Emissivity
    PROP_RHUM = 4,				// Relative Humidity
    PROP_TREF = 5,				// Reference Temperature
    PROP_EOTEMP = 6,			// External Optics Temperature (in Kelvin)
    PROP_EOTAO = 7,				// External Optics Transmission
    PROP_SCLO = 8,				// Scale Limit Low
    PROP_SCHI = 9,				// Scale Limit High
    PROP_TUNIT = 10,			// Presentation Temperature Unit
    PROP_DUNIT = 11,			// Presentation Distance Unit
    PROP_RANGE = 12,			// Measurement Range
    PROP_FOCUS = 13,			// Focus State (far/near/stop)
    PROP_FOCUS_POS = 14,		// Focus Position 
	PROP_COOLER = 15,			// Cooler State (on/off/standby/cooling)
	PROP_BATTERY = 16,			// Battery State
	PROP_PALETTE = 17,			// Camera palette
	PROP_FILTER_POS = 18,		// Filter wheel position (only THV 900)
	PROP_FILTER_NAME = 19,		// Filter name
	PROP_FILTER_CASS = 20,		// Filter cassette (only THV 900)
	PROP_CAMERA_NAME = 21,		// Camera model name
	PROP_CAMERA_TYPE = 22,		// Camera type
	PROP_LENS_NAME = 23,		// Camera lens description
	PROP_DEVICE = 24,			// Device type
	PROP_PORT = 25,				// Device port
	PROP_VIDEO = 26,			// Video mode
	PROP_STORE_COND = 27,		// Recording store Condition
	PROP_STOP_COND = 28,		// Recording stop Condition
	PROP_START_COND = 29,		// Recording start Condition
	PROP_REC_STATE = 30,		// Recording state (0=disabled/1=waiting/2=recording/3=paused)
	PROP_TRIG_SOURCE = 31,		// Recording trig source
	PROP_TRIG_PORT = 32,		// Recording trig port
	PROP_REC_FORMAT = 33,		// Recording file Format (0=Multiple proprietary image files 2=Multiple public files (FPF))
	PROP_START_VALUE = 34,		// Start value dependends on start condition
	PROP_STORE_VALUE = 35,		// Store value dependends on store condition
	PROP_STOP_VALUE = 36,		// Stop value dependends on stop condition
	PROP_BASE_NAME = 37,		// File base name when storing images on disk
	PROP_REC_PRES = 38,			// Image presentation mode while recording
	PROP_REC_PATH = 39,			// Recording directory path
	PROP_GET_FILES = 40,		// List of recorded files in the current session
	PROP_NOISE_RED = 41,		// Camera noise reduction
	PROP_ZOOM = 42,				// Zoom factor
	PROP_FRAME_RATE = 43,		// Frame rate depends on camera type
	PROP_MOTOR = 44,			// Scanning mirrors motor state (only THV 900). 1=Off 2=On 4=Standby
	PROP_CALIB = 45,			// Calibration title (read only)
	PROP_RANGE_LIST = 46,		// List of valid measurement ranges
	PROP_AUTO_SHUTTER = 47,		// Auto shutter mode (off=0/on=1)
	PROP_FIELD_MODE = 48,		// Field mode (only ThermaCAM SC 2000)
	PROP_SCALE = 49,			// Scale visibility (off=0/on=1)
	PROP_OFFSET_CORR = 50,		// Offset correction (only THV 900 and THV 1000)
	PROP_APERTURE = 51,			// Aperture (only THV 900)
	PROP_IRFILE = 53,			// Disk image source file (can be uses when device type is NONE)
	PROP_TOOLTIP = 54,			// Tooltips (off=0/on=1)
	PROP_ESTIM_TAO = 56,		// Estimated transmission factor
	PROP_REC_INDEX = 57,		// Recording file index
	PROP_ROTATION = 58,			// Image rotation (0=No rotation/1=Horizontal/2=Vertical/3=Diagonal)
	PROP_REC_EXTENSION = 59,	// Recording file extension type
	PROP_PALETTE_LIST = 60,		// List of available camera palettes
	PROP_DEFAULT_INIT = 61,		// Enable=1/disable=0 default camera initialization
	PROP_TEXTCOMMENTS = 62,		// Text comments
	PROP_RESOURCE_PATH = 63,	// Resource path (only for camera type 4, 5,11 and 12)
	PROP_RESOURCE_VALUE = 64,	// Resource value (only for camera type 4, 5, 11 and 12)
	PROP_CAMID = 65,			// Camera ID (only for camera type 5)
	PROP_IMAGE_WIDTH = 66,		// Image horizontal line width in pixels
	PROP_IMAGE_HEIGHT = 67,		// Image height in lines (or rows)
	PROP_IMAGE_PIXSIZE = 68,	// Image pixel size (0 = 16-bit, 1 = 8-bit)
	PROP_CAMERA_STATUS = 69,	// Camera status
	PROP_VIEWER = 70,			// Image viewer (off=0, on=1, show_property_page=2)
	PROP_IMAGE_MODE = 71,		// 16-bit image mode (1=temperature 0=signal (default))
	PROP_MULTICAST = 72,		// 0=Use unicast 1=Use multicast connection (device type 4 and 6) 
	PROP_LINE_CORR = 73,		// 0=No line corr. 1=Enable line corr. (only THV 1000)
	PROP_TRANSFER = 74,			// 0=No transfer 1=Transfer L&S and object pars. to camera
	PROP_FRAME_RATE_LIST = 75,	// List of available frame rates
	PROP_IMAGE_COUNT = 76,		// Number of images in current image sequence file (read only)
	PROP_IMAGE_NUMBER = 77,		// Current image number in image sequence file
	PROP_IMAGE_WRAP = 78,		// TRUE=wrap to first image in sequence
	PROP_DOWNSAMPLE = 79,		// FALSE=No downsample, TRUE=Downsample image (from upsampled image)
	PROP_CAMERA_SN = 80,		// Camera serial number
	PROP_FG_HANDLE = 81,		// Handle to frame grabber board(IRFLashLink or ITEX)
	PROP_IMAGE_CORR = 82,		// 0=No corr. 1=Emissivity corr. enabled 2=Distance corr. enabled
	PROP_OVERRIDE_OP = 83,		// 1=Override source object par. 0=Source decides object parameters
	PROP_OVERRIDE_LS = 84,		// 1=Override source scale par. 0=Source decides scaling
	PROP_SILENT_MODE = 85,		// 1=Silent mode (no popups) 0=Normal
	PROP_HAS_AUTOFOCUS = 86,	// 1=Auto focus present 0=Auto focus not present
	PROP_RESOURCE_SUBSCR = 87,	// Resource subscription (only for Matrix camera type)
	PROP_IMAGE_TIME = 88,		// Current image time stamp as VARIANT type VT_DATE
	PROP_IMAGE_TIME_MSEC = 89,	// Current image time millisecond part VT_I2
	PROP_IMAGE_TRIG_COUNT = 90,	// Current image trig count (VT_I4)
	PROP_CAM_FILE_FORMAT = 91,	// File format for storing images in camera
	PROP_FILE_NAMING = 92,		// File naming used for snapshots 0=Normal 1=Use date and time
	PROP_IMAGE_TIMEOUT = 93,	// Image wait request timeout in ms (VT_I4)
	PROP_CMD_TIMEOUT = 94,		// Command session timeout in ms (VT_I4)
	PROP_SAFE_CONNECT = 95		// TRUE=Safe connection, FALSE=Normal connection
};

// Camera status codes (as returned using PROP_CAMERA_STATUS)
enum CAMSTATUS
{
   CAM_CONNECTED      = 0,
   CAM_DISCONNECTED   = 1,
   CAM_GETTINGINFO    = 2,
   CAM_CONBROKEN      = 3,
   CAM_CLOSING        = 4
};

// Error/Status codes
#define IS_OK				    0
#define IS_DEV_NOT_PRESENT		1
#define IS_DEV_BUSY			    2
#define IS_DEV_DRV_MISSING		3
#define IS_DEV_DRV_VERSION		4		// Driver version old or incorrect
#define IS_DEV_LOAD_FAIL	    5		// Failed to load LCA
#define IS_DEV_CONFIG_ERR		6		// Resource conflict
#define IS_DEV_SCM_ERROR        8       // Service Control Manager error (NT)
#define IS_DEV_CONNECT_FAIL     9       // Failed to connect using serial link
#define IS_DEV_NOT_CONNECTED    10      // Serial link not open
#define IS_NO_BUFFER 			11		// Cannot allocate image buffer
#define IS_INVALID_IMAGE  		12
#define IS_HARDWARE_ERROR	  	13		// Device hardware error
#define IS_TIMEOUT			    14		// Timeout waiting for image
#define IS_CONFIG_FAIL			15		// Camera configuration error
#define IS_ABORTED				16		// Image acquisition aborted
#define IS_NO_INIT				17		// Image source is not initiated

#define IS_DEV_ERR_CONFIG_FILE	20		// Error in image source config file
#define IS_DEV_ERR_INIT		    21		// Failed to initiate device
#define IS_FILE_ACCESS_ERROR	22		// Failed to open or access image file
#define IS_FILE_PATH_ERROR		23		// File path not found
#define IS_FILE_MEDIA_FULL		24		// File media is full. No space left
#define IS_FILE_FORMAT_ERROR	25		// Image file format not recognized
#define IS_EMISS_REFLECTED_ERR	26		// Known temperature is too close to reflected temperature
#define IS_EMISS_STRADDLE_ERR	27		// Known and shown values are on different sides of reflected temperature
#define IS_NOT_SUPPORTED		28
#define IS_ILLEGAL_PARAMETER	29
