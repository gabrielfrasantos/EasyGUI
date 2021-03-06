/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \website http://majerle.eu
 * \license MIT
 * \version 0.1.0
 * \brief   GUI Library
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2017 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GUI_H
#define GUI_H 010

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI EasyGUI
 * \brief           EasyGUI Graphical User Interface for embedded systems
 * \{
 */

/* GUI configuration */
#include "gui_config.h"
    
/* Important GUI definitions */
#include "gui_defs.h"
    
/* Proto threads */
#include "pt/pt.h"
    
/* Include utilities */
#include "utils/gui_buffer.h"
#include "utils/gui_linkedlist.h"
#include "utils/gui_string.h"
#include "utils/gui_timer.h"
#include "utils/gui_math.h"

/* GUI Low-Level drivers */
#include "gui_ll.h"

/**
 * \defgroup        GUI_Internal Internal macros 
 * \{
 */
#define __GUI_ENTER()
#define __GUI_LEAVE()

#define __GUI_DEBUG(fmt, ...)       printf(fmt, ##__VA_ARGS__)

/**
 * \brief           GUI Handle object from main object
 * \retval          Pointer to GUI handle
 * \hideinitializer
 */ 
#define __GH(x)                     ((struct GUI_HANDLE *)(x))

/**
 * \brief           GUI Handle root object from main object with children widgets
 * \retval          Pointer to GUI root handle
 * \hideinitializer
 */ 
#define __GHR(x)                    ((struct GUI_HANDLE_ROOT *)(x))

/**
 * \brief           Allocate memory with specific size in bytes
 * \hideinitializer
 */
#define __GUI_MEMALLOC(size)        malloc(size)

/**
 * \brief           Free memory from specific address previously allocated with \ref __GUI_MEMALLOC
 * \hideinitializer
 */
#define __GUI_MEMFREE(p)            do {            \
    free(p);                                        \
    (p) = NULL;                                     \
} while (0);

/**
 * \brief           Free memory for widget which was just deleted
 * \hideinitializer
 */
#define __GUI_MEMWIDFREE(p)         do {            \
    __GUI_DEBUG("Memory free: 0x%08X; Type: %s\r\n", (uint32_t)__GH(p), __GH(p)->Widget->Name);  \
    memset(p, 0x00, __GH(p)->Widget->Size);         \
    free(p);                                        \
    (p) = 0;                                        \
} while (0)

/**
 * \brief           Check input parameters and return value on failure
 * \hideinitializer
 */
#define __GUI_ASSERTPARAMS(c)       do {            \
    if (!(c)) {                                     \
        __GUI_DEBUG("Assert param failed in file %s and line %d\r\n", __FILE__, __LINE__);  \
        return 0;                                   \
    }                                               \
} while (0)

/**
 * \brief           Check if window is active for widget create
 * \hideinitializer
 */
#define __GUI_ASSERTACTIVEWIN()     do {            \
    if (!GUI.WindowActive) {                        \
        __GUI_DEBUG("There is no active window for widget in file %s on line %d\r\n", __FILE__, __LINE__);  \
        return NULL;                                \
    }                                               \
} while (0)

/**
 * \brief           Get maximal value between 2 values
 * \param[in]       x: First value
 * \param[in]       y: Second value
 * \retval          Maximal value
 * \hideinitializer
 */
#define __GUI_MAX(x, y)             ((x) > (y) ? (x) : (y))

/**
 * \brief           Get minimal value between 2 values
 * \param[in]       x: First value
 * \param[in]       y: Second value
 * \retval          Minimal value
 * \hideinitializer
 */
#define __GUI_MIN(x, y)             ((x) < (y) ? (x) : (y))

/**
 * \brief           Get absolute value of input
 * \param[in]       x: Input value
 * \retval          Absolute value of input
 * \hideinitializer
 */
#define __GUI_ABS(x)                ((x) >= 0 ? (x) : -(x))

/**
 * \brief           Check if 2 rectangle objects covers each other in any way
 * \hideinitializer
 */
#define __GUI_RECT_MATCH(h1x1, h1y1, h1x2, h1y2, h2x1, h2y1, h2x2, h2y2)    \
    !(                                                      \
        (h1x1) > (h2x2) ||                                  \
        (h1y1) > (h2y2) ||                                  \
        (h2x1) > (h1x2) ||                                  \
        (h2y1) > (h1y2)                                  \
    )

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 1 parameter
 * \hideinitializer
 */
#define __GUI_UNUSED(x)             (void)(x)

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 2 parameters
 * \hideinitializer
 */
#define __GUI_UNUSED2(x, y)         { __GUI_UNUSED(x); __GUI_UNUSED(y); }

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 3 parameters
 * \hideinitializer
 */
#define __GUI_UNUSED3(x, y, z)      { __GUI_UNUSED(x); __GUI_UNUSED(y); __GUI_UNUSED(z); }

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 4 parameters
 * \hideinitializer
 */
#define __GUI_UNUSED4(x, y, z, k)   { __GUI_UNUSED(x); __GUI_UNUSED(y); __GUI_UNUSED(z); __GUI_UNUSED(k); }

/**
 * \}
 */

/**
 * \brief           GUI main object structure
 */
typedef struct GUI_t {
    volatile uint32_t Time;                 /*!< Current time in units of milliseconds */
    GUI_LCD_t LCD;                          /*!< LCD low-level settings */
    GUI_LL_t LL;                            /*!< Low-level drawing routines for LCD */
    
    uint32_t Flags;                         /*!< Core GUI flags management */
    
    GUI_Display_t Display;                  /*!< Clipping management */
    GUI_Display_t DisplayTemp;              /*!< Clipping for widgets for drawing and touch */
    
    GUI_HANDLE_p WindowActive;              /*!< Pointer to currently active window when creating new widgets */
    GUI_HANDLE_p FocusedWidget;             /*!< Pointer to focused widget for keyboard events if any */
    GUI_HANDLE_p FocusedWidgetPrev;         /*!< Pointer to previously focused widget */
    
    GUI_LinkedListRoot_t Root;              /*!< Root linked list of widgets */
    GUI_TIMER_CORE_t Timers;                /*!< Software structure management */
    
#if GUI_USE_TOUCH || defined(DOXYGEN)
    __GUI_TouchData_t TouchOld;             /*!< Old touch data, used for event management */
    __GUI_TouchData_t Touch;                /*!< Current touch data and processing tool */
    GUI_HANDLE_p ActiveWidget;              /*!< Pointer to widget currently active by touch */
    GUI_HANDLE_p ActiveWidgetPrev;          /*!< Previously active widget */
#endif /* GUI_USE_TOUCH */
} GUI_t;
#if defined(GUI_INTERNAL)
extern GUI_t GUI;
#endif /* defined(GUI_INTERNAL) */

/* Include widget structure */
#include "widgets/gui_widget.h"
#include "input/gui_input.h"

/**
 * \brief           Initializes GUI stack.
 *                    In addition, it prepares memory for work with widgets on later usage and
 *                    calls low-layer functions to initialize LCD or custom driver for LCD
 * \retval          Member of \ref GUI_Result_t enumeration
 */
GUI_Result_t GUI_Init(void);

/**
 * \brief           Processes all drawing operations for GUI
 * \retval          Number of jobs done in current call
 */
int32_t GUI_Process(void);
void GUI_UpdateTime(uint32_t millis);

//Notify GUI from low-level that layer is in use
void GUI_LCD_ConfirmActiveLayer(GUI_Byte layer_num);
 
/**
 * \} GUI
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
