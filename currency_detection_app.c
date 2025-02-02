#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xaxivdma.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "sleep.h"

// VDMA configuration
#define FRAME_WIDTH  1280
#define FRAME_HEIGHT 720
#define FRAME_SIZE   (FRAME_WIDTH * FRAME_HEIGHT * 3)
#define ROI_WIDTH    224
#define ROI_HEIGHT   224

// DMA buffer addresses (set these according to your memory map)
#define FRAME_BUFFER_BASE   0x10000000
#define ROI_BUFFER_BASE    0x11000000
#define RESULT_BUFFER_BASE 0x12000000

// Hardware instances
static XAxiVdma vdma;
static XScuGic intc;

// Frame buffers
volatile u32 *frame_buffer = (volatile u32 *)FRAME_BUFFER_BASE;
volatile float *roi_buffer = (volatile float *)ROI_BUFFER_BASE;
volatile u32 *result_buffer = (volatile u32 *)RESULT_BUFFER_BASE;

// Currency class messages
const char* class_messages[] = {
    "50 rupees note",
    "500 rupees note",
    "100 rupees note",
    "10 rupees note",
    "20 rupees note",
    "200 rupees note"
};

// Function prototypes
static int setup_hardware(void);
static void process_frame(void);
static void extract_roi(void);
static void handle_result(void);

int main(void) {
    // Initialize system
    if (setup_hardware() != XST_SUCCESS) {
        xil_printf("Hardware setup failed!\r\n");
        return XST_FAILURE;
    }

    xil_printf("Currency Detection System Started\r\n");

    // Main processing loop
    while (1) {
        process_frame();
        extract_roi();
        handle_result();
        usleep(33333); // ~30 fps
    }

    return XST_SUCCESS;
}

static int setup_hardware(void) {
    int status;
    XAxiVdma_Config *vdma_config;

    // Initialize VDMA
    vdma_config = XAxiVdma_LookupConfig(XPAR_AXI_VDMA_0_DEVICE_ID);
    if (!vdma_config) return XST_FAILURE;

    status = XAxiVdma_CfgInitialize(&vdma, vdma_config, vdma_config->BaseAddress);
    if (status != XST_SUCCESS) return XST_FAILURE;

    // Configure VDMA for continuous operation
    XAxiVdma_StartParking(&vdma, 0, XAXIVDMA_READ);
    XAxiVdma_StartParking(&vdma, 0, XAXIVDMA_WRITE);

    // Setup interrupts (if needed)
    // Initialize cache
    Xil_DCacheEnable();
    Xil_ICacheEnable();

    return XST_SUCCESS;
}

static void process_frame(void) {
    // Wait for frame capture complete
    Xil_DCacheInvalidateRange((UINTPTR)frame_buffer, FRAME_SIZE);
    
    // Clear any previous results
    memset((void*)result_buffer, 0, sizeof(u32));
}

static void extract_roi(void) {
    int roi_x = (FRAME_WIDTH - ROI_WIDTH) / 2;
    int roi_y = (FRAME_HEIGHT - ROI_HEIGHT) / 2;
    float *roi_ptr = (float*)roi_buffer;

    // Extract and normalize ROI
    for (int y = 0; y < ROI_HEIGHT; y++) {
        for (int x = 0; x < ROI_WIDTH; x++) {
            int frame_idx = ((roi_y + y) * FRAME_WIDTH + (roi_x + x)) * 3;
            for (int c = 0; c < 3; c++) {
                *roi_ptr++ = frame_buffer[frame_idx + c] / 255.0f;
            }
        }
    }

    Xil_DCacheFlushRange((UINTPTR)roi_buffer, ROI_WIDTH * ROI_HEIGHT * 3 * sizeof(float));
}

static void handle_result(void) {
    // Wait for hardware acceleration to complete
    Xil_DCacheInvalidateRange((UINTPTR)result_buffer, sizeof(u32));
    
    // Get classification result
    u32 predicted_class = *result_buffer;
    if (predicted_class < 6) {
        xil_printf("Detected: %s\r\n", class_messages[predicted_class]);
    }
}
