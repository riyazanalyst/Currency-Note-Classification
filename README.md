# Currency Note Classification Hardware Implementation

## Project Structure

```
Currency-Note-Classification-Vitis/
├── .ipynb_checkpoints/           # Jupyter checkpoint files
├── Dataset/                      # Training dataset
├── output/                       # Model output files
├── hw_implementation/            # Hardware implementation files
│   ├── hls/
│   │   ├── CNC.cpp              # HLS implementation
│   │   └── include/
│   │       └── cnn_params.h     # CNN parameters
│   │
│   ├── vivado/
│   │   ├── constraints/
│   │   │   └── currency_detection.xdc
│   │   ├── ip_repo/
│   │   │   └── cnn_1.0/        # Generated HLS IP
│   │   └── block_design/
│   │       └── system_bd.tcl
│   │
│   └── vitis/
│       ├── src/
│       │   └── currency_detection_app.c
│       └── include/
│           └── app_params.h
│
├── my_model.keras               # Keras model file
├── my_model.onnx               # ONNX model file
└── Currency Note Classification.ipynb    # Training notebook
```

## Directory Description

- **`.ipynb_checkpoints/`**: Contains Jupyter notebook checkpoints
- **`Dataset/`**: Contains training dataset for the model
- **`output/`**: Stores model output files
- **`hw_implementation/`**: Hardware implementation files
  - **`hls/`**: High-Level Synthesis files
  - **`vivado/`**: Vivado project files
  - **`vitis/`**: Vitis application files
- **`my_model.keras`**: Trained Keras model
- **`my_model.onnx`**: ONNX format model
- **`Currency Note Classification.ipynb`**: Main training notebook
