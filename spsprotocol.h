#ifndef SPSPROTOCOL_H
#define SPSPROTOCOL_H

namespace Sps {
// Radar type is 4 for these kind
#define RADAR_TYPE 4

// Units, measurements
#define AZIMUTH_LSB 0.088f
#define RANGE_LSB 0.11575f
#define HEIGHT_LSB 125.0f
#define ELEVATION_LSB 0.088f

// Test command pattern to be sent
#define TEST_PATTERN 0xFFFF

// Some useful definitions
#define GET_SIZE(p)     ((p)->bits.ML)
#define CHECK_SIZE(p)   (GET_SIZE(p) == sizeof((p)->packet) / sizeof(quint16))
#define SET_SIZE(p)     ((p)->bits.ML = sizeof((p)->packet) / sizeof(quint16))

// Reported messages def
enum ReportID
{
    PRIMARY_PLOT        = 0x00, // 0000
    SECONDARY_PLOT      = 0x01, // 0001
    COMBINED_PLOT       = 0x02, // 0010
    JAMMING_STROBE      = 0x03, // 0011
    // Reserved           0x04, // 0100
    // Spares (Growth)    0x05, // 0101 user
    STATUS              = 0x06, // 0110
    // Spares (Growth)    0x07, // 0111 user
    AZIMUTH_REF         = 0x08, // 1000
    TEST                = 0x09, // 1001
    TBM_TRACK           = 0x0A, // 1010
    FULL_ABT_TRACK      = 0x0B, // 1011 user
    PERIODIC_ABT_TRACK  = 0x0C, // 1100 user
    CANCEL_ABT_TRACK    = 0x0D  // 1101 user
    // Spares (Growth)    0x0E, // 1110 user
    // Spares (Growth)    0x0F, // 1111 user
};

enum ANTENNA_ROTATION
{
    RPM_6 = 0,
    RPM_12 = 1,
    NOT_USED = 2,
    STOPPED = 3
};

// Validate given report ID
#define CHECK_REPORT_ID(id) ((4 > (id)) || (6 == (id)) || (7 < (id) && 14 > (id)))

// Define the first word of messages (message header word)
#define DEFINE_MESSAGE_HEADER \
            /* word0 */ \
            quint16 ML  : 4; /* Message length */ \
            quint16 RN  : 3; /* Radar identification number */ \
            quint16 TTI : 1; /* Specify if test target, 0 - not a test target, 1 - test target */ \
            quint16 RT  : 4; /* Radar type, 4 is the default value */ \
            quint16 RID : 4; /* Report identification */

//// Define the first word of messages (message header word)
//#define DEFINE_COMMAND_HEADER \
//            /* word0 */ \
//            quint16 ML  : 4; /* Message length */ \
//            quint16 RN  : 3; /* Radar identification number */ \
//            quint16 S   : 1; /* Specify if test target, 0 - not a test target, 1 - test target */ \
//            quint16 unused : 4; \
//            quint16 CID : 4; /* Command identification */

// Some useful definitions to get message header fields
#define DEFINE_MESSAGE_HEADER_FUNCTIONS \
    quint8 getRadarType() const { return bits.RT; } \
    bool testTargetIndicator() const { return bits.TTI; } \
    quint8 getRadarID() const { return 1 + bits.RN; } \
    quint8 getLength() const { return bits.ML; } \
    QByteArray getData() const { return QByteArray(reinterpret_cast<const char*>(&bits), sizeof(bits)); } \
    bool checkRadarID(int radarID) const { \
        auto flag = getRadarID() == radarID; \
        if (! flag) { \
        /*    qWarning() << "Received message radar number is wrong: expected " <<  */ \
        /*        radarID << ", but got " << getRadarNumber(); */ \
        } \
        return flag; \
    }

// Geo coordinates getter functions
#define DEFINE_GEOCOORDINATE_GETTER_FUNCTIONS \
    float getAzimuth() const { return bits.azimuth * AZIMUTH_LSB; } \
    float getRange() const { return bits.range * RANGE_LSB; /* km */ } \
    float getHeight() const { return bits.height * HEIGHT_LSB; /* feet */ }

// Commad IDS to be sent
enum CommandID
{
   EMCON_RADAR_ON   = 0x00,
   EMCON_RADAR_OFF  = 0x01,
   EMCON_IFF_ON     = 0x02,
   EMCON_IFF_OFF    = 0x03,
   IFF_MODE_SELECT  = 0x04,
   NSM_CONTROL      = 0x08,
};

// Primary plot
struct PrimaryPlotMessage
{
    explicit PrimaryPlotMessage() : bits()
    {
        bits.RID = PRIMARY_PLOT;
        SET_SIZE(this);
    }

    union
    {
        quint16 packet[4];
        struct
        {
            // word0
            DEFINE_MESSAGE_HEADER

            // word1
            quint16 azimuth : 12; // Straigh binary natural number with 0.088 degree for each
            quint16 unused0 : 4;

            // word2
            quint16 range   : 14; // Slant binary number with 0.11575 lsb for each
            quint16 unused1 : 2;

            // word3
            qint16 height   : 16; // target height, signed with 0.125 feet (0.3048)
        } bits;
    };

    DEFINE_MESSAGE_HEADER_FUNCTIONS

    DEFINE_GEOCOORDINATE_GETTER_FUNCTIONS

    bool validate() const {
        return CHECK_SIZE(this) && PRIMARY_PLOT == this->bits.RID;
    }

    QString getName() const {
       return "Primary Plot Report";
    }
};

// Status of radar, once per scan or upon change
struct StatusMessage
{
    explicit StatusMessage() : bits()
    {
        bits.RID = STATUS;
        bits.TTI = 0;
        SET_SIZE(this);
    }

    union
    {
        quint16 packet[3];
        struct
        {
            // word0
            DEFINE_MESSAGE_HEADER

            // word1
            quint16 RR      : 2; // Radar radiation mode, 0 - off, 1 - 1st stage, 2 = driver stage, 3 - final stage
            quint16 RPM     : 2; // Antenna rotation, 0 - 6RPM, 1 - 12RPM, 2 - Not used, 3 - Stopped
            quint16 ANT     : 1; // Antenna status, 0 - dummy load, 1 - Antenna
            quint16 BS      : 1; // Battleshoot enabled, 0 == off, 1 == on
            quint16 SGE     : 1; // Scan mode gate enabled
            quint16 TEG     : 3; // Total emission gate active??
            quint16 TTE     : 1; // Test target generation is enabled
            quint16 RCC     : 3; // Radar control console is assigned as local radar control, 0 - local operator console 1, 1 - LOC2, 2 - Remote operator console 1, 3 ROC..
            quint16 FILT    : 1; // Targets reported to EADC are filtered
            quint16 OVRL    : 1; // Overload condition, track capacity reached, 0 - no overload, 1 - overload

            // word2
            quint16 RFS     : 2; // Radar fault status, 00, 01 - On and up, 10 = Degraded, 11 - off or desabled
            quint16 IFS     : 2; // IFF fault status, 00, 01 - On and up, 10 = Degraded, 11 - off or desabled
            quint16 SMR0    : 1; // secure mode, reserved
            quint16 CZI     : 1; // NSM code zeroize indicator, 0 - not zerizied, 1 - zeroized
            quint16 SMR1    : 3; // secure mode, reserved
            quint16 IF      : 1; // interface fault, radar command validation error, 1 - Fault
            quint16 LR      : 1; // Local or remote mode, local = autonomous mode, remote - system accepts commands from EADC
            quint16 unused  : 1;
            quint16 MCE     : 1; // Mode C is enabled
            quint16 M3E     : 1; // Mode3 enable status
            quint16 M2E     : 1; // Mode2 enable status
            quint16 M1E     : 1; // Mode1 enable status
        } bits;
    };

    DEFINE_MESSAGE_HEADER_FUNCTIONS

    bool validate() const {
        return CHECK_SIZE(this) && STATUS == this->bits.RID;
    }

    QString getName() const {
       return "Status Report";
    }
};

struct AzimuthReferenceMessage
{
    explicit AzimuthReferenceMessage () : bits()
    {
        bits.RID = AZIMUTH_REF;
        bits.TTI = 0;
        SET_SIZE(this);
    }

    union
    {
        quint16 packet[1];
        struct
        {
            // word0
            DEFINE_MESSAGE_HEADER
        } bits;
    };

    DEFINE_MESSAGE_HEADER_FUNCTIONS

    bool validate() const {
        return CHECK_SIZE(this) && AZIMUTH_REF == this->bits.RID;
    }

    QString getName() const {
        return "Azimuth Reference Report";
    }
};

/**
 * Test Command is used to test the operational status of the
 * interface toghether with the Test report
 */

struct TestMessage
{
    explicit TestMessage() : bits() {}

    // Test target or not? TTI
    explicit TestMessage (quint16 TTI) : bits()
    {
        bits.RID = TEST;
        bits.RT = RADAR_TYPE;
        bits.TTI = TTI;
        bits.pattern = TEST_PATTERN;
        bits.RN = 0;
        SET_SIZE(this);
    }

    // The fields can be accessed outerside
    union
    {
        quint16 packet[2];
        struct
        {
            // word0
            DEFINE_MESSAGE_HEADER

            // word1
            quint16 pattern: 16; // Command test pattern
        } bits;
    };

public:
    DEFINE_MESSAGE_HEADER_FUNCTIONS

    bool validate() const {
        return CHECK_SIZE(this) && TEST == this->bits.RID /**&& TEST_PATTERN == this->bits.pattern*/;
    }

    QString getName() const {
        return "Test Report";
    }
};
}

#endif // SPSPROTOCOL_H
