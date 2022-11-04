// Originally forked from https://github.com/jfedor2/gamepad/ , but departed so much it became it's own thing
// Author: CyberDNIWE
// Debugging is a pain with no real step-through, so use these macroses to easily remove printouts from sources at will
// Just coment out defines like DEBUG_PRINT_ENABLED to remove ALL printouts (suggested for final build)
// Or each individual DEBUG_PRINT_XXXXX to remove specific ones. (useful for debugging)

//#define BUILD_SIMULATOR_NOUSB
//#define DEBUG_PRINT_ENABLED BUILD_SIMULATOR_NOUSB
//#define BEBUG_PRINT_BUTTONS 
//#define DEBUG_PRINT_BUTTONS_RELEASED
//#define BEBUG_PRINT_DEBOUNCE_STATE 
//#define BEBUG_PRINT_DPAD 
//#define DEBUG_PRINT_USB_REPORT 
//#define DEBUG_PRINT_SOCD
#ifndef BUILD_SIMULATOR_NOUSB
  #include <HID.h>
#endif
#include <Arduino.h>
// Debug printout macros
#define REMOVED_FROM_SOURCE ;
#ifdef DEBUG_PRINT_ENABLED
# define debugPrintf(str) Serial.println(str);
# ifdef BEBUG_PRINT_BUTTONS
#   define debugPrintf_BUTTONS(str) Serial.println(str);
# else
#   define debugPrintf_BUTTONS(str) REMOVED_FROM_SOURCE
# endif
# ifdef BEBUG_PRINT_DEBOUNCE_STATE
#   define debugPrintf_DEBOUNCE(str) Serial.println(str);
# else
#   define debugPrintf_DEBOUNCE(str) REMOVED_FROM_SOURCE
# endif
# ifdef BEBUG_PRINT_DPAD
#   define debugPrintf_DPAD(str) Serial.println(str);
# else
#   define debugPrintf_DPAD(str) REMOVED_FROM_SOURCE
# endif
# ifdef DEBUG_PRINT_USB_REPORT
#   define debugPrintf_USB(str) Serial.println(str);
# else
#   define debugPrintf_USB(str) REMOVED_FROM_SOURCE
# endif
# ifdef DEBUG_PRINT_SOCD
#   define debugPrintf_SOCD(str) Serial.println(str);
# else
#   define debugPrintf_SOCD(str) REMOVED_FROM_SOURCE
# endif
# ifdef DEBUG_PRINT_BUTTONS_RELEASED
#   define debugPrintf_BUTTONS_RELEASED(str) Serial.println(str);
# else
#   define debugPrintf_BUTTONS_RELEASED(str) REMOVED_FROM_SOURCE
# endif
#else
# define debugPrintf(str)                  REMOVED_FROM_SOURCE
# define debugPrintf_BUTTONS(str)          REMOVED_FROM_SOURCE
# define debugPrintf_DEBOUNCE(str)         REMOVED_FROM_SOURCE
# define debugPrintf_DPAD(str)             REMOVED_FROM_SOURCE
# define debugPrintf_USB(str)              REMOVED_FROM_SOURCE
# define  debugPrintf_SOCD(str)            REMOVED_FROM_SOURCE
# define debugPrintf_BUTTONS_RELEASED(str) REMOVED_FROM_SOURCE
#endif

// SOCD cleaners are: (tournamentLegal being the default at start)
//     socd_strategies::none
//     socd_strategies::tournamentLegal
//     socd_strategies::allNeutral
//     socd_strategies::lastInputPriority
#define SOCD_STRATEGY_SYCLE_DELAY 1
#define SOCD_STRATEGY_CYCLE_BUTTONS &PS, &Start

// You can now switch SOCD type in real-time using SOCD_STRATEGY_CYCLE_BUTTONS to do so.
// Switching to next available SOCD method will only happen if all listed buttons are pressed
// And will wait SOCD_STRATEGY_SYCLE_DELAY worth of seconds before switching to next one if not released by then
// The selected SOCD type is reset to default upon disconnecting.

// To change SOCD_STRATEGY_CYCLE_BUTTONS you should list all the buttons needed to activate the switch (order independent)
// divided by comma (,) with ampersant (&) before their name like so: &PS, &Start
// Possible button names are:
//  X
//  Square  
//  Sircle  
//  Triangle
//  L1      
//  R1      
//  L2      
//  R2      
//  Select  
//  Start   
//  PS      


// Default debounce amount in ms
#define DEBOUNCE_DEFAULT_MS 5

// Potentially customizable debounce timing for every button
#define DEBOUNCE_BUT_X              DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_SQUARE         DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_CIRCLE         DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_TRIANGLE       DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_L1             DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_R1             DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_L2             DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_R2             DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_SELECT         DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_START          DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_PS             DEBOUNCE_DEFAULT_MS
// Same for D-pad
#define DEBOUNCE_BUT_DPAD_UP        DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_DPAD_DOWN      DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_DPAD_LEFT      DEBOUNCE_DEFAULT_MS
#define DEBOUNCE_BUT_DPAD_RIGHT     DEBOUNCE_DEFAULT_MS


// I used an Arduino Pro Micro board, but it should work with any ATmega32U4-based board, just set the right pins below.
// If you want L3/R3, the PS button or analog sticks, you'll need to add them.

namespace board_config
{
  // Adjust your pins here:
  enum enPins_Buttons : unsigned char 
  {
    //D-pad
    PIN_UP        = 2,
    PIN_DOWN      = 3,
    PIN_LEFT      = 4,
    PIN_RIGHT     = 5,
    //Others
    PIN_CROSS     = 6,
    PIN_CIRCLE    = 7,
    PIN_TRIANGLE  = 8,
    PIN_SQUARE    = 9,
    PIN_L1        = 10,
    // For Micro Pro:
    //*
    PIN_L2        = 14,
    PIN_R1        = 16,
    PIN_R2        = 15,
    PIN_SELECT    = 18, // A0
    PIN_START     = 19, // A1
    PIN_PS        = 20  // A2
    //*/

    // For UNO (this is mostly for wokwi emulator debuging, UNO has no USBHID compatibility):    
    /*
    PIN_L2        = 11,
    PIN_R1        = 12,
    PIN_R2        = 13,
    // At this point we're out of digital pins on UNO
    PIN_SELECT    = 14, // A0
    PIN_START     = 15, // A1
    PIN_PS        = 16  // A2
    //*/
  };
};
using enPinsBUTTONS = board_config::enPins_Buttons;

// HID report descriptor taken from a HORI Fighting Stick V3
// works out of the box with PC and PS3
// as dumped by usbhid-dump and parsed by https://eleccelerator.com/usbdescreqparser/
static const uint8_t hidReportDescriptor[] PROGMEM =
{
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x05,        // Usage (Game Pad)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x01,        // Report ID, not in the original descriptor, but the Arduino HID library can't seem to live without it
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x35, 0x00,        //   Physical Minimum (0)
  0x45, 0x01,        //   Physical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x0D,        //   Report Count (13)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x0D,        //   Usage Maximum (0x0D)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x03,        //   Report Count (3)
  0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x25, 0x07,        //   Logical Maximum (7)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
  0x09, 0x39,        //   Usage (Hat switch)
  0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
  0x65, 0x00,        //   Unit (None)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x46, 0xFF, 0x00,  //   Physical Maximum (255)
  0x09, 0x30,        //   Usage (X)
  0x09, 0x31,        //   Usage (Y)
  0x09, 0x32,        //   Usage (Z)
  0x09, 0x35,        //   Usage (Rz)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x04,        //   Report Count (4)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x20,        //   Usage (0x20)
  0x09, 0x21,        //   Usage (0x21)
  0x09, 0x22,        //   Usage (0x22)
  0x09, 0x23,        //   Usage (0x23)
  0x09, 0x24,        //   Usage (0x24)
  0x09, 0x25,        //   Usage (0x25)
  0x09, 0x26,        //   Usage (0x26)
  0x09, 0x27,        //   Usage (0x27)
  0x09, 0x28,        //   Usage (0x28)
  0x09, 0x29,        //   Usage (0x29)
  0x09, 0x2A,        //   Usage (0x2A)
  0x09, 0x2B,        //   Usage (0x2B)
  0x95, 0x0C,        //   Report Count (12)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x0A, 0x21, 0x26,  //   Usage (0x2621)
  0x95, 0x08,        //   Report Count (8)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x0A, 0x21, 0x26,  //   Usage (0x2621)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
  0x46, 0xFF, 0x03,  //   Physical Maximum (1023)
  0x09, 0x2C,        //   Usage (0x2C)
  0x09, 0x2D,        //   Usage (0x2D)
  0x09, 0x2E,        //   Usage (0x2E)
  0x09, 0x2F,        //   Usage (0x2F)
  0x75, 0x10,        //   Report Size (16)
  0x95, 0x04,        //   Report Count (4)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              // End Collection
};

enum bfButtonHID : uint16_t
{
  HID_SQUARE     = (1u << 0),
  HID_CROSS      = (1u << 1),
  HID_CIRCLE     = (1u << 2),     
  HID_TRIANGLE   = (1u << 3),       
  HID_L1         = (1u << 4), 
  HID_R1         = (1u << 5), 
  HID_L2         = (1u << 6), 
  HID_R2         = (1u << 7), 
  HID_SELECT     = (1u << 8),     
  HID_START      = (1u << 9),  
  HID_L3         = (1u << 10), 
  HID_R3         = (1u << 11),
  HID_PS         = (1u << 12)
};

// 0 = up, 1 = up/right, 2 = right etc., 0x0f = neutral
enum DpadHID : uint8_t
{
  DPAD_UP       = 0,    //0b00000000
  DPAD_DOWN     = 4,    //0b00000100
  DPAD_LEFT     = 6,    //0b00000110
  DPAD_RIGHT    = 2,    //0b00000010
  DPAD_NEUTRAL  = 0x0f  //0b11111111
};

struct hid_report_t
{
  uint16_t buttons; // bits: 0 = square, 1 = cross, 2 = circle, 3 = triangle,
                    // 4 = L1, 5 = R1, 6 = L2, 7 = R2, 8 = select, 9 = start, 12 = PS
                    // 10 = L3?, 11 = R3?
  uint8_t dpadHat; // 0 = up, 1 = up/right, 2 = right etc., 0x0f = neutral
  uint8_t leftStickXAxis;
  uint8_t leftStickYAxis;
  uint8_t rightStickXAxis;
  uint8_t rightStickYAxis;
  uint8_t dpadRightAxis;
  uint8_t dpadLeftAxis;
  uint8_t dpadUpAxis;
  uint8_t dpadDownAxis;
  uint8_t triangleAxis;
  uint8_t circleAxis;
  uint8_t crossAxis;
  uint8_t squareAxis;
  uint8_t L1Axis;
  uint8_t L2Axis;
  uint8_t R1Axis;
  uint8_t R2Axis;
  uint16_t accelerometerXAxis; // 10 bits (these are guesses BTW)
  uint16_t accelerometerYAxis; // 10 bits
  uint16_t accelerometerZAxis; // 10 bits
  uint16_t gyroscopeAxis; // 10 bits
};

inline bool operator!=(const hid_report_t& lhs, const hid_report_t& rhs) noexcept
{
  return memcmp(&lhs, &rhs, sizeof(rhs));
}
inline bool operator==(const hid_report_t& lhs, const hid_report_t& rhs) noexcept
{
  return !(lhs != rhs);
}


constexpr hid_report_t _make_report() noexcept
{
  return
  {
    /* buttons;              =    */  0x00,
    /* dpadHat;              =    */  0x0f,
    /* leftStickXAxis;       =    */  0x80,
    /* leftStickYAxis;       =    */  0x80,
    /* rightStickXAxis;      =    */  0x80,
    /* rightStickYAxis;      =    */  0x80,
    /* dpadRightAxis;        =    */  0x00,
    /* dpadLeftAxis;         =    */  0x00,
    /* dpadUpAxis;           =    */  0x00,
    /* dpadDownAxis;         =    */  0x00,
    /* triangleAxis;         =    */  0x00,
    /* circleAxis;           =    */  0x00,
    /* crossAxis;            =    */  0x00,
    /* squareAxis;           =    */  0x00,
    /* L1Axis;               =    */  0x00,
    /* L2Axis;               =    */  0x00,
    /* R1Axis;               =    */  0x00,
    /* R2Axis;               =    */  0x00,
    /* accelerometerXAxis;   =    */  0x02, // not sure why 0x02, but that's what HORI Fighting Stick V3 sends
    /* accelerometerYAxis;   =    */  0x02, // not sure why 0x02, but that's what HORI Fighting Stick V3 sends
    /* accelerometerZAxis;   =    */  0x02, // not sure why 0x02, but that's what HORI Fighting Stick V3 sends
    /* gyroscopeAxis;        =    */  0x02  // not sure why 0x02, but that's what HORI Fighting Stick V3 sends
  };
}

class Reporter
{
public:
  hid_report_t m_report;
  hid_report_t m_prevReport;

  constexpr Reporter(): m_report(_make_report()), m_prevReport(_make_report())
  {}

  void resetReport()
  {
    m_report.buttons = 0x00;
    m_report.triangleAxis = 0x00;
    m_report.circleAxis = 0x00;
    m_report.crossAxis = 0x00;
    m_report.squareAxis = 0x00;
    m_report.L1Axis = 0x00;
    m_report.L2Axis = 0x00;
    m_report.R1Axis = 0x00;
    m_report.R2Axis = 0x00;
  }

  void sendReportIfNeeded()
  {
    if(m_report != m_prevReport)
    {
      m_prevReport = m_report;
#     ifndef BUILD_SIMULATOR_NOUSB
      HID().SendReport(1, &m_report, sizeof(m_report));
#     endif
      debugPrintf_USB("Sending NEW REPORT!");
    }
  }

  hid_report_t& getReportBuffer() noexcept
  {
    return m_report;
  }
};


// Setting up and checking pins helpers
template<typename PIN>
inline void setupButtonPin(PIN pin) noexcept
{
  debugPrintf("Btn setup");
  pinMode(pin, INPUT_PULLUP);
}

// Instead of repeating "digitalRead(pin) == LOW" use this:
template<typename PIN>
inline bool buttonIsPressed(PIN pin) noexcept
{
  return digitalRead(pin) == LOW;
}

// Not used, maybe #Todo: make dpad mode switchable
/* 
enum enStickMode : unsigned char 
{
  DPAD = 0,
  LEFT_STICK = 1
};
*/

// For HID report, we need bit operations
template<typename TRGT, typename BF>
inline void setBit(TRGT& target, BF mask) noexcept
{
  target |= mask;
}

template<typename TRGT, typename BF>
inline void clearBit(TRGT& target, BF mask) noexcept
{
  target &= ~mask;
}

// Few helper templates to abstract bit operations even more
template<typename BTN, typename BF, typename AXIS>
inline void regButtonPressed(BTN& b, AXIS& a, BF mask) noexcept
{
  setBit(b, mask);
  a = 0xff;
}

template<typename BTN, typename BF, typename AXIS>
inline void regButtonReleased(BTN& b, AXIS& a, BF mask) noexcept
{
  clearBit(b, mask);
  a = 0x00;
}

template<typename BTN, typename BF>
inline void regButtonPressed(BTN& b, BF mask) noexcept
{
  setBit(b, mask);
}

template<typename BTN, typename BF>
inline void regButtonReleased(BTN& b, BF mask) noexcept
{
  clearBit(b, mask);
}


// Ignore _inner namespace, it's for poor man's array
namespace _inner
{
  template<typename DT>
  struct MyArray
  {
    const DT** data;
    const size_t size;
  };
  
  template <typename T, size_t N>
  constexpr size_t array_size(const T(&)[N]) noexcept { return N; }
};

class HidReportable
{
  public:
    constexpr HidReportable() noexcept = default;
    virtual ~HidReportable() = default;
    
    virtual void init() const noexcept = 0; // const-init will make sense later ;)
    virtual void reportTo(hid_report_t& target) const noexcept = 0;

  protected:
};

// Base class for Buttons, unfourtunately no RAII - pins need setting up after creation
class ButtonBase : public HidReportable
{
  public:
    constexpr ButtonBase(enPinsBUTTONS pin) noexcept : m_pin(pin)
    {}

    virtual ~ButtonBase() = default;
    virtual void init() const noexcept override
    {
      setupButtonPin(m_pin);
    }

    virtual bool btnIsPressed() const noexcept
    {
      return buttonIsPressed(m_pin);
    }
  
  protected:
    const enPinsBUTTONS m_pin;
};

// Two way debouncer to debounce a button on press and on release.
// Works also on sub-ms level (when two updates come within same millis())
// Note: if you find a nicer way of doing this, feel free to open pull request
class Debouncer
{
  public:
    constexpr Debouncer(unsigned long debounceTime) noexcept : m_pressTime(0UL), m_debounceTime(debounceTime), m_previous(false)
    {}

    bool debounce(bool actuallyPressed) noexcept
    {
      const auto now = getTimeSinceStart();
      
      if(m_previous != actuallyPressed)
      {
        debugPrintf_DEBOUNCE("Debouncer: detected state change");
        if(m_pressTime)
        {
          if(now > (m_pressTime + m_debounceTime))
          {
            debugPrintf_DEBOUNCE("Debouncer: reseting debounce time");
            m_previous = actuallyPressed;
            m_pressTime = now;
          }
        }
        else
        {
          debugPrintf_DEBOUNCE("Debouncer: registering state change, setting debounce time");
          m_previous = actuallyPressed;
          m_pressTime = now;
        }



      #ifdef DEBUG_PRINT_ENABLED
        if(m_previous)
        {
          debugPrintf_DEBOUNCE("Debouncer: sending PRESSED");
        }
        else
        {
          debugPrintf_DEBOUNCE("Debouncer: sending RELEASED");
        }
      #endif
        
      }

      return m_previous;
    }    
    
  protected:
    virtual unsigned long getTimeSinceStart() const noexcept
    {
      return millis();
    }


    unsigned long m_pressTime           = 0;
    const unsigned long m_debounceTime  = 0;
    bool m_previous                     = false;
};

// Debounced button - just a regular ButtonBase that has its press debounced
class ButtonDebounced : public ButtonBase
{
  public:
    constexpr ButtonDebounced(enPinsBUTTONS pin, unsigned long debounceTime = DEBOUNCE_DEFAULT_MS) noexcept : 
    ButtonBase(pin), m_debouncer(debounceTime)
    {}
    virtual ~ButtonDebounced() = default;

    virtual bool btnIsPressed() const noexcept override
    {
      return m_debouncer.debounce(this->::ButtonBase::btnIsPressed());
    }

  protected:    
    mutable Debouncer m_debouncer;
};

// Base class for D-pad button (also debounced)
class Btn_Dpad : public ButtonDebounced
{
  public:
    constexpr Btn_Dpad(enPinsBUTTONS pin, unsigned long debounceMS = DEBOUNCE_DEFAULT_MS) : ButtonDebounced(pin, debounceMS)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    { /* does not report directly */ }
};

struct DirectionBF
{
	bool up     : 1;
	bool down   : 1;
	bool left   : 1;
	bool right  : 1;
};

// Simultanious Opposite Cardinal Directions (SOCD) cleaning strategies
namespace cleaner_strategy
{
  struct SOCD_CleaningStrategy
  {
    virtual void clean(DirectionBF& directions) const noexcept = 0;
  };

  struct None : public SOCD_CleaningStrategy
  {
    virtual void clean(DirectionBF& directions) const noexcept 
    {
      debugPrintf_SOCD("NO SOCD APPLIED!");
    };
  };

  struct TournamentLegal : public SOCD_CleaningStrategy
  {
    virtual void clean(DirectionBF& directions) const noexcept override
    {
      if(directions.up && directions.down)
      {        
        directions.down = false;
        debugPrintf_SOCD("Tournament legal SOCD: UP + DOWN = UP");
      }
      if(directions.left && directions.right)
      {        
        directions.left  = false;
        directions.right = false;
        debugPrintf_SOCD("Tournament legal SOCD: LEFT + RIGHT = NEUTRAL");
      }
    }
  };

  struct AllNeutral : public SOCD_CleaningStrategy
  {
    virtual void clean(DirectionBF& directions) const noexcept override
    {      
      if(directions.up && directions.down)
      {        
        directions.up = false;
        directions.down = false;
        debugPrintf_SOCD("All neutral SOCD: UP + DOWN = NEUTRAL");
      }
      if(directions.left && directions.right)
      {        
        directions.left = false;
        directions.right = false;
        debugPrintf_SOCD("All neutral SOCD: LEFT + RIGHT = NEUTRAL");
      }
    }
  };
  
  struct LastInputPriority : public SOCD_CleaningStrategy
  {
    mutable DirectionBF m_prevs = { false, false, false, false };

    virtual void clean(DirectionBF& directions) const noexcept override
    {
      cleanCardinal_Vertical(directions);
      cleanCardinal_Horizontal(directions);
    }

    inline void cleanCardinal_Horizontal(DirectionBF& directions) const noexcept
    {
      // Clean left/right
      if(directions.left && directions.right)
      {
        if(directions.left && m_prevs.right)
        {
          debugPrintf_SOCD("LastInputPriority SOCD: RIGHT + LEFT = LEFT");
          directions.right = false;
        }

        if(directions.right && m_prevs.left)
        {
          debugPrintf_SOCD("LastInputPriority SOCD: LEFT + RIGHT = RIGHT");
          directions.left = false;
        }
      }
      else
      {
        m_prevs.left   = directions.left;
        m_prevs.right  = directions.right;
      }
    }
    inline void cleanCardinal_Vertical(DirectionBF& directions)   const noexcept
    {
      //Clean up/down
      if(directions.up && directions.down)
      {
        if(directions.up && m_prevs.down)
        {
          debugPrintf_SOCD("LastInputPriority SOCD: DOWN + UP = UP");
          directions.down = false;
        }

        if(directions.down && m_prevs.up)
        {
          debugPrintf_SOCD("LastInputPriority SOCD: UP + DOWN = DOWN");
          directions.up = false;
        }
      }
      else
      {
        m_prevs.up   = directions.up;
        m_prevs.down = directions.down;
      }
    }
  };
  
  template<typename CMB_CHKR>
  struct StrategySwitcher : public SOCD_CleaningStrategy
  {
    public:

    constexpr StrategySwitcher(const SOCD_CleaningStrategy* const* strategies, size_t size, const CMB_CHKR& combination, unsigned long nextPressTimeoutSecondsAmt = SOCD_STRATEGY_SYCLE_DELAY) :
        m_strategies(strategies), m_currentStrategy(strategies[0] ? strategies[0] : nullptr), 
        m_isComboCurrentlyPressed(combination), m_currentIdx(0), m_strategies_size(size),
        m_activeTimeoutAmt(nextPressTimeoutSecondsAmt * 1000UL), m_becomesActiveAt(0UL)
    {}

    virtual void clean(DirectionBF& directions) const noexcept
    {
        switchIfNeeded();
        if(m_currentStrategy)
        {
            m_currentStrategy->clean(directions);
        }
    }

    void switchIfNeeded() const noexcept
    {
        const auto now = getTimeSinceStart();
        if(m_becomesActiveAt <= now)
        {
            if(m_isComboCurrentlyPressed())
            {
                m_becomesActiveAt = now + m_activeTimeoutAmt;
                switchToNextStrategy();
            }
        }
    }

    protected:
    virtual unsigned long getTimeSinceStart() const noexcept
    {
      return millis();
    }

    private:
    
    inline void switchToNextStrategy() const noexcept
    {
        
        size_t idx = m_currentIdx + 1;
        
        if(idx >= m_strategies_size)
        {
            idx = 0;
        }

        if(idx < m_strategies_size)
        {
            m_currentStrategy = m_strategies[idx];
            m_currentIdx = idx;
        }
    }

    const   SOCD_CleaningStrategy* const   *  m_strategies;
    const   SOCD_CleaningStrategy  mutable *  m_currentStrategy;
    const   CMB_CHKR&  m_isComboCurrentlyPressed;

    mutable size_t          m_currentIdx;
    const   size_t          m_strategies_size;
    const   unsigned long   m_activeTimeoutAmt;
    mutable unsigned long   m_becomesActiveAt;
  };

};


// For convenience and readability: define Dpad's constructor parameters with their default values
#define _DPAD_CTOR_PARAMS \
const cleaner_strategy::SOCD_CleaningStrategy* cleaner = nullptr, \
enPinsBUTTONS pin_up     = enPinsBUTTONS::PIN_UP,      unsigned long btn_up_debuonceMs    = DEBOUNCE_DEFAULT_MS, \
enPinsBUTTONS pin_down   = enPinsBUTTONS::PIN_DOWN,    unsigned long btn_down_debuonceMs  = DEBOUNCE_DEFAULT_MS, \
enPinsBUTTONS pin_left   = enPinsBUTTONS::PIN_LEFT,    unsigned long btn_left_debuonceMs  = DEBOUNCE_DEFAULT_MS, \
enPinsBUTTONS pin_right  = enPinsBUTTONS::PIN_RIGHT,   unsigned long btn_right_debuonceMs = DEBOUNCE_DEFAULT_MS

// Concrete D-pad: composit(s) dpad buttons, uses SOCD cleaning
class Dpad : public HidReportable
{
  public:
    constexpr Dpad(_DPAD_CTOR_PARAMS) noexcept :
    m_btn_up(pin_up, btn_up_debuonceMs),       m_btn_down(pin_down, btn_down_debuonceMs),
    m_btn_left(pin_left, btn_left_debuonceMs), m_btn_right(pin_right, btn_right_debuonceMs),
    m_cleaner(cleaner)
    {}

    virtual void init() const noexcept override
    {
      m_btn_up.init();
      m_btn_down.init();
      m_btn_left.init();
      m_btn_right.init();
    }

  protected:
    Btn_Dpad m_btn_up;
    Btn_Dpad m_btn_down;
    Btn_Dpad m_btn_left;
    Btn_Dpad m_btn_right;
    const cleaner_strategy::SOCD_CleaningStrategy*  m_cleaner;
  
    virtual void reportTo(hid_report_t& target) const noexcept override
    {
      // Gather all button states (automatically debounces, no need to worry here)
      DirectionBF directions = 
      {
        m_btn_up.btnIsPressed(),
        m_btn_down.btnIsPressed(),
        m_btn_left.btnIsPressed(),
        m_btn_right.btnIsPressed()
      };

      // Clean states up according to cleaning strategy
      if(m_cleaner)
      {
        m_cleaner->clean(directions);
      }
      #ifdef DEBUG_PRINT_ENABLED
      else
      {
        debugPrintf_SOCD("NO SOCD CLEANER FOUND!");
      }
      #endif

      // Fill report
      if(directions.up && !directions.right && !directions.left) 
      {
        target.dpadHat = 0;
        debugPrintf_DPAD("Pressed d-pad: ↑");
      }
      else if(directions.up && directions.right)
      {
        target.dpadHat = 1;
        debugPrintf_DPAD("Pressed d-pad: ↗");
      }
      else if(directions.right && !directions.up && !directions.down)
      {        
        target.dpadHat = 2;
        debugPrintf_DPAD("Pressed d-pad: →");
      }
      else if(directions.right && directions.down)
      {
        target.dpadHat = 3;
        debugPrintf_DPAD("Pressed d-pad: ↘");
      }
      else if(directions.down && !directions.right && !directions.left)
      {
        target.dpadHat = 4;
        debugPrintf_DPAD("Pressed d-pad: ↓");
      }
      else if(directions.down && directions.left)
      {
        target.dpadHat = 5;
        debugPrintf_DPAD("Pressed d-pad: ↙");
      }
      else if(directions.left && !directions.down && !directions.up)
      {
        target.dpadHat = 6;
        debugPrintf_DPAD("Pressed d-pad: ←");
      }
      else if(directions.left && directions.up)
      {
        target.dpadHat = 7;
        debugPrintf_DPAD("Pressed d-pad: ↖");
      }
      else
      {
        //debugPrintf_DPAD("Pressed d-pad: NEUTRAL");
        target.dpadHat = 0x0f;
      }

      // Not sure if needed, but original had these so might as well
      target.dpadRightAxis = directions.right ? 0xff : 0x00;
      target.dpadLeftAxis  = directions.left  ? 0xff : 0x00;
      target.dpadUpAxis    = directions.up    ? 0xff : 0x00;
      target.dpadDownAxis  = directions.down  ? 0xff : 0x00;
    }
};

#undef _DPAD_CTOR_PARAMS


//========== Concrete buttons ==============//
// Here we have a class declarations and definitions for every button
class Btn_X : public ButtonDebounced
{
  public:
    constexpr Btn_X(enPinsBUTTONS pin = enPinsBUTTONS::PIN_CROSS, unsigned long debounceMs = DEBOUNCE_BUT_X) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_CROSS;
      auto& axis    = target.crossAxis;
      auto& buttons = target.buttons;
      
      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  X");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  X");
      }
    }
};

class Btn_SQUARE : public ButtonDebounced
{
  public:
    constexpr Btn_SQUARE(enPinsBUTTONS pin = enPinsBUTTONS::PIN_SQUARE, unsigned long debounceMs = DEBOUNCE_BUT_SQUARE) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_SQUARE;
      auto& axis    = target.squareAxis;
      auto& buttons = target.buttons;

      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  □");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  □");
      }
    }
};

class Btn_CIRCLE : public ButtonDebounced
{
  public:
    constexpr Btn_CIRCLE(enPinsBUTTONS pin = enPinsBUTTONS::PIN_CIRCLE, unsigned long debounceMs = DEBOUNCE_BUT_CIRCLE) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_CIRCLE;
      auto& axis    = target.circleAxis;
      auto& buttons = target.buttons;

      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  o");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  o");
      }
    }
};

class Btn_TRIANGLE : public ButtonDebounced
{
  public:
    constexpr Btn_TRIANGLE(enPinsBUTTONS pin = enPinsBUTTONS::PIN_TRIANGLE, unsigned long debounceMs = DEBOUNCE_BUT_TRIANGLE) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_TRIANGLE;
      auto& axis    = target.triangleAxis;
      auto& buttons = target.buttons;

      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  △");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  △");
      }
    }
};

class Btn_L1 : public ButtonDebounced
{
  public:
    constexpr Btn_L1(enPinsBUTTONS pin = enPinsBUTTONS::PIN_L1, unsigned long debounceMs = DEBOUNCE_BUT_L1) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_L1;
      auto& axis    = target.L1Axis;
      auto& buttons = target.buttons;

      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  L1");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  L1");
      }
    }
};

class Btn_R1 : public ButtonDebounced
{
  public:
    constexpr Btn_R1(enPinsBUTTONS pin = enPinsBUTTONS::PIN_R1, unsigned long debounceMs = DEBOUNCE_BUT_R1) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_R1;
      auto& buttons = target.buttons;
      auto& axis    = target.R1Axis;
      
      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  R1");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  R1");
      }
    }
};

class Btn_L2 : public ButtonDebounced
{
  public:
    constexpr Btn_L2(enPinsBUTTONS pin = enPinsBUTTONS::PIN_L2, unsigned long debounceMs = DEBOUNCE_BUT_L2) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_L2;
      auto& buttons = target.buttons;
      auto& axis    = target.L2Axis;
      
      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  L2");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  L2");
      }
    }
};

class Btn_R2 : public ButtonDebounced
{
  public:
    constexpr Btn_R2(enPinsBUTTONS pin = enPinsBUTTONS::PIN_R2, unsigned long debounceMs = DEBOUNCE_BUT_R2) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_R2;
      auto& buttons = target.buttons;
      auto& axis    = target.R2Axis;
      
      if(btnIsPressed())
      {
        regButtonPressed(buttons, axis, mask);
        debugPrintf_BUTTONS("Button Pressed:  R2");
      }
      else
      {
        regButtonReleased(buttons, axis, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  R2");
      }
    }
};

class Btn_SELECT : public ButtonDebounced
{
  public:
    constexpr Btn_SELECT(enPinsBUTTONS pin = enPinsBUTTONS::PIN_SELECT, unsigned long debounceMs = DEBOUNCE_BUT_SELECT) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_SELECT;
      auto& buttons = target.buttons;
      
      if(btnIsPressed())
      {
        regButtonPressed(buttons, mask);
        debugPrintf_BUTTONS("Button Pressed:  SELECT");
      }
      else
      {
        regButtonReleased(buttons, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  SELECT");
      }
    }
};

class Btn_START : public ButtonDebounced
{
  public:
    constexpr Btn_START(enPinsBUTTONS pin = enPinsBUTTONS::PIN_START, unsigned long debounceMs = DEBOUNCE_BUT_START) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_START;
      auto& buttons = target.buttons;
      
      if(btnIsPressed())
      {
        regButtonPressed(buttons, mask);
        debugPrintf_BUTTONS("Button Pressed:  START");
      }
      else
      {
        regButtonReleased(buttons, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  START");
      }
    }
};

class Btn_PS : public ButtonDebounced
{
  public:
    constexpr Btn_PS(enPinsBUTTONS pin = enPinsBUTTONS::PIN_PS, unsigned long debounceMs = DEBOUNCE_BUT_PS) noexcept : ButtonDebounced(pin, debounceMs)
    {}

  protected:
    virtual void reportTo(hid_report_t& target) const noexcept 
    {
      constexpr auto mask = bfButtonHID::HID_PS;
      auto& buttons = target.buttons;

      if(btnIsPressed())
      {
        regButtonPressed(buttons, mask);
        debugPrintf_BUTTONS("Button Pressed:  [PS]");
      }
      else
      {
        regButtonReleased(buttons, mask);
        debugPrintf_BUTTONS_RELEASED("Button Released:  [PS]");
      }
    }
};

//==========================================//


// Template for root of our button array (we will eventually place them all in one)
template<typename ARR>
struct Root
{
  public:
    constexpr Root(const ARR& arr) noexcept : m_arr(arr)
    {};

    void init() const noexcept
    {
      for(size_t i = 0; i < m_arr.size; ++i)
      {
        m_arr.data[i]->init();
      }
    };
    
    void gather(hid_report_t& target) const noexcept
    {
      for(size_t i = 0; i < m_arr.size; ++i)
      {
        m_arr.data[i]->reportTo(target);
      }
    }

  protected:
    const ARR& m_arr;
};

// Gamepad class: responsible for polling everything into a report
template<typename ARR>
struct _Gamepad : public Root<ARR>
{
  public:
    constexpr _Gamepad(const ARR& arr) noexcept : Root<ARR>(arr), m_usbReporter()
    {}

    void pollEverything() noexcept
    {
      this->gather(m_usbReporter.getReportBuffer());
      m_usbReporter.sendReportIfNeeded();
    }

  protected:
    Reporter m_usbReporter;

};

// Constexpr factory function for _Gamepad creation to make using it appear as if it was just a normal constructor
template<typename ARR>
constexpr _Gamepad<ARR> Gamepad(const ARR& buttons) noexcept
{
  return _Gamepad<ARR>(buttons);
}

// At this point we're done with declarations/definitions and it's time to finally create some objects!

// All SOCD strategies objects reside here, use SOCD_STRATEGY_CYCLE_BUTTONS macro to select the one you like
// Default is tournamentLegal
namespace socd_strategies
{
  using namespace cleaner_strategy;
  static constexpr cleaner_strategy::TournamentLegal    tournamentLegal   = {};
  static constexpr cleaner_strategy::AllNeutral         allNeutral        = {};
  static constexpr cleaner_strategy::LastInputPriority  lastInputPriority = {};
  static constexpr cleaner_strategy::None               none              = {};

  static constexpr const cleaner_strategy::SOCD_CleaningStrategy* strategies[] = 
  {
    &tournamentLegal,
    &allNeutral,
    &lastInputPriority,
    &none
  };
};



// All Buttons objects reside here
namespace buttons_storage
{
  static const auto X         = Btn_X();
  static const auto Square    = Btn_SQUARE();
  static const auto Sircle    = Btn_CIRCLE();
  static const auto Triangle  = Btn_TRIANGLE();
  static const auto L1        = Btn_L1();
  static const auto R1        = Btn_R1();
  static const auto L2        = Btn_L2();
  static const auto R2        = Btn_R2();
  static const auto Select    = Btn_SELECT();
  static const auto Start     = Btn_START();
  static const auto PS        = Btn_PS();

  // Make socd switcher that switches based on button combination given
  static const      ButtonBase* _switcher_buttons[] = { SOCD_STRATEGY_CYCLE_BUTTONS };
  constexpr size_t _switcher_buttons_size = _inner::array_size(_switcher_buttons);
  static const      auto  comboChecker = +[]()
  {
    bool ret = _switcher_buttons_size;
    if(ret)
    {
        for(size_t i = 0; i < _switcher_buttons_size; ++i)
        {
            const auto* btn =_switcher_buttons[i];
            if(btn && !btn->btnIsPressed())
            {
                ret = false;
                break;
            }
        }
    }
    return ret;
  };

  static constexpr  cleaner_strategy::StrategySwitcher<decltype(comboChecker)> socdSwitcher = 
  { 
    socd_strategies::strategies, _inner::array_size(socd_strategies::strategies), 
    comboChecker
  };
  
  // Make d-pad with socd switcher
  static const auto dpad      = Dpad(&socdSwitcher);
};

// Here resides an array of all buttons as their HidReportable base
// as well as some relevant stuff
namespace all
{
  // Simplify adding/removing buttons by putting them into array
  namespace _raw_array
  {
    using namespace buttons_storage;
    // Add button pointers to this array
    static const HidReportable* _buttons[] = 
    {
      &X,
      &Square,
      &Sircle,
      &Triangle,
      &L1,
      &R1,
      &L2,
      &R2,
      &Select,
      &Start,
      &PS,
      &dpad
    };
  };

  // Just use this buttons array for initialization
  static constexpr _inner::MyArray<HidReportable> buttons = {_raw_array::_buttons, _inner::array_size(_raw_array::_buttons)};
};



// Here is our Gamepad instance, that uses all buttons
static auto g_gamepad = Gamepad(all::buttons);
// Now to actually put the thing together with arduino funtions
void setup()
{
  // Just created magic static HIDSubDescriptor and initialize g_gamepad
#   ifndef BUILD_SIMULATOR_NOUSB
    static HIDSubDescriptor node(hidReportDescriptor, sizeof(hidReportDescriptor));
    HID().AppendDescriptor(&node);
#   endif
#   ifdef DEBUG_PRINT_ENABLED
    Serial.begin(115200); // Any baud rate should work
    debugPrintf("Serail Initialized!\n");
#   endif

  g_gamepad.init();
}

// In every loop just tell g_gamepad to poll everything.
void loop()
{
  g_gamepad.pollEverything();
}
