#ifndef _ESP32_RGB_64_32_MATRIX_PANEL_I2S_DMA
#define _ESP32_RGB_64_32_MATRIX_PANEL_I2S_DMA

/***************************************************************************************/
/* COMPILE-TIME OPTIONS - CONFIGURE AS DESIRED                                         */
/***************************************************************************************/

/* Enable serial debugging of the library, to see how memory is allocated etc. */
//#define SERIAL_DEBUG 1

/* Split the framebuffer into two smaller memory allocations. Can allow for 
 * bigger resolution due to the fragmented memory map of the typical Arduino sketch 
 * even before setup(). Enabled by default.
 */
#define SPLIT_MEMORY_MODE 1

/* Use GFX_Root (https://github.com/mrfaptastic/GFX_Root) instead of 
 * Adafruit_GFX library. No real benefit unless you don't want Bus_IO library. 
 */
//#define USE_GFX_ROOT 1


/* Physical / Chained HUB75(s) RGB pixel WIDTH and HEIGHT. 
 *
 * This library has only been tested with a 64 pixel (wide) and 32 (high) RGB panel. 
 * Theoretically, if you want to chain two of these horizontally to make a 128x32 panel
 * you can do so with the cable and then set the MATRIX_WIDTH to '128'.
 *
 * Also, if you use a 64x64 panel, then set the MATRIX_HEIGHT to '64' and an E_PIN; it will work!
 *
 * All of this is memory permitting of course (dependant on your sketch etc.) ...
 *
 */
#ifndef MATRIX_HEIGHT
	#define MATRIX_HEIGHT               32 
#endif

#ifndef MATRIX_WIDTH
	#define MATRIX_WIDTH                64
#endif

#ifndef PIXEL_COLOR_DEPTH_BITS
	#define PIXEL_COLOR_DEPTH_BITS      8   // 8bit per RGB color = 24 bit/per pixel, reduce to save RAM
#endif

#ifndef MATRIX_ROWS_IN_PARALLEL
	#define MATRIX_ROWS_IN_PARALLEL     2   // Don't change this unless you know what you're doing
#endif


/* ESP32 Default Pin definition. You can change this, but best if you keep it as is and provide custom pin mappings 
 * as part of the begin(...) function.
 */
#define R1_PIN_DEFAULT  22
#define G1_PIN_DEFAULT  32
#define B1_PIN_DEFAULT  21
#define R2_PIN_DEFAULT  19
#define G2_PIN_DEFAULT  33
#define B2_PIN_DEFAULT  17

#define A_PIN_DEFAULT   16
#define B_PIN_DEFAULT   25
#define C_PIN_DEFAULT   27
#define D_PIN_DEFAULT   26
#define E_PIN_DEFAULT   -1 // IMPORTANT: Change to a valid pin if using a 64x64px panel.
          
#define LAT_PIN_DEFAULT 14
#define OE_PIN_DEFAULT  13
#define CLK_PIN_DEFAULT 12

// Interesting Fact: We end up using a uint16_t to send data in parallel to the HUB75... but 
//                   given we only map to 14 physical output wires/bits, we waste 2 bits.

/***************************************************************************************/
/* Do not change.                                                                      */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_heap_caps.h"
#include "esp32_i2s_parallel.h"

#ifdef USE_GFX_ROOT
	#include "GFX.h" // Adafruit GFX core class -> https://github.com/mrfaptastic/GFX_Root
#else	
	#include "Adafruit_GFX.h" // Adafruit class with all the other stuff
#endif	

/***************************************************************************************/
/* Do not change.                                                                      */

// Panel Upper half RGB (numbering according to order in DMA gpio_bus configuration)
#define BIT_R1  (1<<0)   
#define BIT_G1  (1<<1)   
#define BIT_B1  (1<<2)   

// Panel Lower half RGB
#define BIT_R2  (1<<3)   
#define BIT_G2  (1<<4)   
#define BIT_B2  (1<<5)   

// Panel Control Signals
#define BIT_LAT (1<<6) 
#define BIT_OE  (1<<7)  

// Panel GPIO Pin Addresses (A, B, C, D etc..)
#define BIT_A (1<<8)    
#define BIT_B (1<<9)    
#define BIT_C (1<<10)   
#define BIT_D (1<<11)   
#define BIT_E (1<<12)   

// RGB Panel Constants / Calculated Values
#define COLOR_CHANNELS_PER_PIXEL 3 
#define PIXELS_PER_ROW ((MATRIX_WIDTH * MATRIX_HEIGHT) / MATRIX_HEIGHT) // = 64
//#define PIXEL_COLOR_DEPTH_BITS (MATRIX_COLOR_DEPTH/COLOR_CHANNELS_PER_PIXEL)  //  = 8
#define ROWS_PER_FRAME (MATRIX_HEIGHT/MATRIX_ROWS_IN_PARALLEL) //  = 16

/***************************************************************************************/
/* Keep this as is. Do not change.                                                     */
#define ESP32_I2S_DMA_MODE          I2S_PARALLEL_BITS_16    // Pump 16 bits out in parallel
#define ESP32_I2S_DMA_STORAGE_TYPE  uint16_t                // one uint16_t at a time.
//#define ESP32_I2S_CLOCK_SPEED     (20000000UL)            // @ 20Mhz
#define ESP32_I2S_CLOCK_SPEED       (10000000UL)  // @ 10Mhz
#define CLKS_DURING_LATCH            0   // Not used. 
/***************************************************************************************/            


/* rowBitStruct
 * Note: sizeof(data) must be multiple of 32 bits, as ESP32 DMA linked list buffer address pointer 
 *       must be word-aligned.
 */
struct rowBitStruct {
    ESP32_I2S_DMA_STORAGE_TYPE data[PIXELS_PER_ROW + CLKS_DURING_LATCH]; 
    // This evaluates to just data[64] really.. an array of 64 uint16_t's
};

/* rowColorDepthStruct
 * Duplicates of row bit structure, but for each color 'depth'ness. 
 */
struct rowColorDepthStruct {
    rowBitStruct rowbits[PIXEL_COLOR_DEPTH_BITS];
};

/* frameStruct
 * Note: This 'frameStruct' will contain ALL the data for a full-frame as BOTH 2x16-row frames are
 *       are contained in parallel within the one uint16_t that is sent in parallel to the HUB75. 
 */
#ifdef SPLIT_MEMORY_MODE
//#pragma message("Split DMA Memory Allocation Mode Enabled")
#define SPLIT_MEMORY_ROWS_PER_FRAME (ROWS_PER_FRAME/2)
struct frameStruct {
    rowColorDepthStruct rowdata[SPLIT_MEMORY_ROWS_PER_FRAME];
};
#else
struct frameStruct {
    rowColorDepthStruct rowdata[ROWS_PER_FRAME];
};
#endif

typedef struct rgb_24 {
    rgb_24() : rgb_24(0,0,0) {}
    rgb_24(uint8_t r, uint8_t g, uint8_t b) {
        red = r; green = g; blue = b;
    }
    rgb_24& operator=(const rgb_24& col);

    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_24;


/***************************************************************************************/   
#ifdef USE_GFX_ROOT
class RGB64x32MatrixPanel_I2S_DMA : public GFX {
#else
class RGB64x32MatrixPanel_I2S_DMA : public Adafruit_GFX {	
#endif

  // ------- PUBLIC -------
  public:
    
    /**
     * RGB64x32MatrixPanel_I2S_DMA 
     * 
     * @param  {bool} _double_buffer : Double buffer is disabled by default. Enable only if you know what you're doing. Manual switching required with flipDMABuffer() and showDMABuffer()
     *        
     */
    RGB64x32MatrixPanel_I2S_DMA(bool _double_buffer = false)
#ifdef USE_GFX_ROOT	
      : GFX(MATRIX_WIDTH, MATRIX_HEIGHT), double_buffering_enabled(_double_buffer)  {
#else
      : Adafruit_GFX(MATRIX_WIDTH, MATRIX_HEIGHT), double_buffering_enabled(_double_buffer)  {
#endif		  

    }

    /* Propagate the DMA pin configuration, or use compiler defaults */
    bool begin(int dma_r1_pin = R1_PIN_DEFAULT , int dma_g1_pin = G1_PIN_DEFAULT, int dma_b1_pin = B1_PIN_DEFAULT , int dma_r2_pin = R2_PIN_DEFAULT , int dma_g2_pin = G2_PIN_DEFAULT , int dma_b2_pin = B2_PIN_DEFAULT , int dma_a_pin  = A_PIN_DEFAULT  , int dma_b_pin = B_PIN_DEFAULT  , int dma_c_pin = C_PIN_DEFAULT , int dma_d_pin = D_PIN_DEFAULT  , int dma_e_pin = E_PIN_DEFAULT , int dma_lat_pin = LAT_PIN_DEFAULT, int dma_oe_pin = OE_PIN_DEFAULT , int dma_clk_pin = CLK_PIN_DEFAULT)
    {

      // Change 'if' to '1' to enable, 0 to not include this Serial output in compiled program        
      #if SERIAL_DEBUG       
            Serial.printf("Using pin %d for the R1_PIN\n", dma_r1_pin);
            Serial.printf("Using pin %d for the G1_PIN\n", dma_g1_pin);
            Serial.printf("Using pin %d for the B1_PIN\n", dma_b1_pin);
            Serial.printf("Using pin %d for the R2_PIN\n", dma_r2_pin);
            Serial.printf("Using pin %d for the G2_PIN\n", dma_g2_pin);
            Serial.printf("Using pin %d for the B2_PIN\n", dma_b2_pin);
            Serial.printf("Using pin %d for the A_PIN\n", dma_a_pin);   
            Serial.printf("Using pin %d for the B_PIN\n", dma_b_pin);   
            Serial.printf("Using pin %d for the C_PIN\n", dma_c_pin);   
            Serial.printf("Using pin %d for the D_PIN\n", dma_d_pin);   
            Serial.printf("Using pin %d for the E_PIN\n", dma_e_pin);                      
            Serial.printf("Using pin %d for the LAT_PIN\n", dma_lat_pin);   
            Serial.printf("Using pin %d for the OE_PIN\n",  dma_oe_pin);    
            Serial.printf("Using pin %d for the CLK_PIN\n", dma_clk_pin); 
      #endif   

     /* As DMA buffers are dynamically allocated, we must allocated in begin()
      * Ref: https://github.com/espressif/arduino-esp32/issues/831
      */
      if ( !allocateDMAmemory() ) {  return false; } // couldn't even get the basic ram required.
        

      // Flush the DMA buffers prior to configuring DMA - Avoid visual artefacts on boot.
      clearScreen(); // Must fill the DMA buffer with the initial output bit sequence or the panel will display garbage
      flipDMABuffer(); // flip to backbuffer 1
      clearScreen(); // Must fill the DMA buffer with the initial output bit sequence or the panel will display garbage
      flipDMABuffer(); // backbuffer 0
      
      // Setup the ESP32 DMA Engine. Sprite_TM built this stuff.
      configureDMA(dma_r1_pin, dma_g1_pin, dma_b1_pin, dma_r2_pin, dma_g2_pin, dma_b2_pin, dma_a_pin,  dma_b_pin, dma_c_pin, dma_d_pin, dma_e_pin, dma_lat_pin,  dma_oe_pin,   dma_clk_pin ); //DMA and I2S configuration and setup

      showDMABuffer(); // show backbuf_id of 0

      #if SERIAL_DEBUG 
        if (!everything_OK)    
              Serial.println("RGB64x32MatrixPanel_I2S_DMA::begin() failed.");
      #endif      

      return everything_OK;

    }
    
    // TODO: Disable/Enable auto buffer flipping (useful for lots of drawPixel usage)...

    // Draw pixels
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);   // overwrite adafruit implementation
    virtual void fillScreen(uint16_t color);                        // overwrite adafruit implementation
            void clearScreen() { fillScreen(0); } 
    void drawPixelRGB565(int16_t x, int16_t y, uint16_t color);
    void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b);
    void drawPixelRGB24(int16_t x, int16_t y, rgb_24 color);
    void drawIcon (int *ico, int16_t x, int16_t y, int16_t cols, int16_t rows);
    
    // Color 444 is a 4 bit scale, so 0 to 15, color 565 takes a 0-255 bit value, so scale up by 255/15 (i.e. 17)!
    uint16_t color444(uint8_t r, uint8_t g, uint8_t b) { return color565(r*17,g*17,b*17); }

    // Converts RGB888 to RGB565
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b); // This is what is used by Adafruit GFX!
    
    // Converts RGB333 to RGB565
    uint16_t Color333(uint8_t r, uint8_t g, uint8_t b); // This is what is used by Adafruit GFX! Not sure why they have a capital 'C' for this particular function.

    inline void flipDMABuffer() 
    {         
      if ( !double_buffering_enabled) return;
        
        // Flip to other buffer as the backbuffer. i.e. Graphic changes happen to this buffer (but aren't displayed until showDMABuffer())
        back_buffer_id ^= 1; 
        
        #if SERIAL_DEBUG     
                Serial.printf("Set back buffer to: %d\n", back_buffer_id);
        #endif      

        // Wait before we allow any writing to the buffer. Stop flicker.
        while(!i2s_parallel_is_previous_buffer_free()) {}       
    }
    
    inline void showDMABuffer()
    {
      
        if (!double_buffering_enabled) return;

        #if SERIAL_DEBUG     
                Serial.printf("Showtime for buffer: %d\n", back_buffer_id);
        #endif      
      
        i2s_parallel_flip_to_buffer(&I2S1, back_buffer_id);

        // Wait before we allow any writing to the buffer. Stop flicker.
        while(!i2s_parallel_is_previous_buffer_free()) {}               
    }
    
    
    inline void setPanelBrightness(int b)
    {
      // Change to set the brightness of the display, range of 1 to matrixWidth (i.e. 1 - 64)
        brightness = b;
    }
    
    inline void setMinRefreshRate(int rr)
    {
        min_refresh_rate = rr;
    } 

  int  calculated_refresh_rate  = 0;         
            
    
   // ------- PRIVATE -------
  private:

    /* Pixel data is organized from LSB to MSB sequentially by row, from row 0 to row matrixHeight/matrixRowsInParallel 
     * (two rows of pixels are refreshed in parallel) */
    frameStruct *matrix_framebuffer_malloc_1; 

#ifdef SPLIT_MEMORY_MODE
    /* In the case there's a 2nd non-contiguous block of DMA capable SRAM we can use, let us try and use that as well. */
    frameStruct *matrix_framebuffer_malloc_2; 
#endif

    // ESP 32 DMA Linked List descriptor
    int desccount = 0;
    lldesc_t * dmadesc_a = {0}; 
    lldesc_t * dmadesc_b = {0};

    // ESP32-RGB64x32MatrixPanel-I2S-DMA functioning
    bool everything_OK              = false;
    bool double_buffering_enabled   = false;// Do we use double buffer mode? Your project code will have to manually flip between both.
    int  back_buffer_id             = 0;    // If using double buffer, which one is NOT active (ie. being displayed) to write too?
    int  brightness           = 32;             // If you get ghosting... reduce brightness level. 60 seems to be the limit before ghosting on a 64 pixel wide physical panel for some panels.
    int  min_refresh_rate     = 120;            // Probably best to leave as is unless you want to experiment. Framerate has an impact on brightness and also power draw - voltage ripple.
    int  lsbMsbTransitionBit  = 0;     // For possible color depth calculations
    
    /* Calculate the memory available for DMA use, do some other stuff, and allocate accordingly */
    bool allocateDMAmemory();

    /* Setup the DMA Link List chain and initiate the ESP32 DMA engine */
    void configureDMA(int r1_pin, int  g1_pin, int  b1_pin, int  r2_pin, int  g2_pin, int  b2_pin, int  a_pin, int   b_pin, int  c_pin, int  d_pin, int  e_pin, int  lat_pin, int   oe_pin, int clk_pin); // Get everything setup. Refer to the .c file
   
    /* Update a specific pixel in the DMA buffer to a colour */
    void updateMatrixDMABuffer(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue);
   
    /* Update the entire DMA buffer (aka. The RGB Panel) a certain colour (wipe the screen basically) */
    void updateMatrixDMABuffer(uint8_t red, uint8_t green, uint8_t blue);       

}; // end Class header

/***************************************************************************************/   
// https://stackoverflow.com/questions/5057021/why-are-c-inline-functions-in-the-header
/* 2. functions declared in the header must be marked inline because otherwise, every translation unit which includes the header will contain a definition of the function, and the linker will complain about multiple definitions (a violation of the One Definition Rule). The inline keyword suppresses this, allowing multiple translation units to contain (identical) definitions. */
inline void RGB64x32MatrixPanel_I2S_DMA::drawPixel(int16_t x, int16_t y, uint16_t color) // adafruit virtual void override
{
  drawPixelRGB565( x, y, color);
} 

inline void RGB64x32MatrixPanel_I2S_DMA::fillScreen(uint16_t color)  // adafruit virtual void override
{
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
  
  updateMatrixDMABuffer(r, g, b); // the RGB only (no pixel coordinate) version of 'updateMatrixDMABuffer'
} 

// For adafruit
inline void RGB64x32MatrixPanel_I2S_DMA::drawPixelRGB565(int16_t x, int16_t y, uint16_t color) 
{
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
  
  updateMatrixDMABuffer( x, y, r, g, b);
}

inline void RGB64x32MatrixPanel_I2S_DMA::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b) 
{
  updateMatrixDMABuffer( x, y, r, g, b);
}

inline void RGB64x32MatrixPanel_I2S_DMA::drawPixelRGB24(int16_t x, int16_t y, rgb_24 color) 
{
  updateMatrixDMABuffer( x, y, color.red, color.green, color.blue);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
//https://github.com/squix78/ILI9341Buffer/blob/master/ILI9341_SPI.cpp
inline uint16_t RGB64x32MatrixPanel_I2S_DMA::color565(uint8_t r, uint8_t g, uint8_t b) {

/*
  Serial.printf("Got r value of %d\n", r);
  Serial.printf("Got g value of %d\n", g);
  Serial.printf("Got b value of %d\n", b);
  */
  
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Promote 3/3/3 RGB to Adafruit_GFX 5/6/5 RRRrrGGGgggBBBbb
inline uint16_t RGB64x32MatrixPanel_I2S_DMA::Color333(uint8_t r, uint8_t g, uint8_t b) { 

    return ((r & 0x7) << 13) | ((r & 0x6) << 10) | ((g & 0x7) << 8) | ((g & 0x7) << 5) | ((b & 0x7) << 2) | ((b & 0x6) >> 1);
    
}


inline void RGB64x32MatrixPanel_I2S_DMA::drawIcon (int *ico, int16_t x, int16_t y, int16_t cols, int16_t rows) {
/*  drawIcon draws a C style bitmap.  
//  Example 10x5px bitmap of a yellow sun 
//
  int half_sun [50] = {
      0x0000, 0x0000, 0x0000, 0xffe0, 0x0000, 0x0000, 0xffe0, 0x0000, 0x0000, 0x0000,
      0x0000, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0x0000,
      0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000,
      0xffe0, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0xffe0,
      0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000,
  };
  
  RGB64x32MatrixPanel_I2S_DMA matrix;

  matrix.drawIcon (half_sun, 0,0,10,5);
*/

  int i, j;
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      drawPixelRGB565 (x + j, y + i, ico[i * cols + j]);
    }
  }  
}

#endif
