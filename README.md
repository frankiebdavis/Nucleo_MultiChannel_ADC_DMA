# STM32 Multi-Channel ADC with DMA (LDR + Potentiometer)

This project uses the **STM32F303RE Nucleo board** to read two analog inputs — a light-dependent resistor (LDR) and a 10kΩ potentiometer — using **DMA-based ADC with interrupt callbacks**. The readings are streamed over UART to a serial terminal, showing how both signals change in real time.

---

## Problem

How can two analog sensors be read **simultaneously and efficiently** on an STM32 without polling each channel manually?

---

## Method

- **Multi-channel ADC1 with DMA:** Two inputs scanned continuously (PA0 and PA1).  
- **DMA Buffer:** Stores both readings in `rawValues[2]`.  
- **Interrupt Callback:** `HAL_ADC_ConvCpltCallback()` sets a flag once both readings are updated.  
- **UART2 Output:** Formatted sensor values are transmitted at 38400 baud.  
- **Main Loop:** Copies ADC values from the DMA buffer and prints them once per second.

Key code excerpt:  
HAL_ADC_Start_DMA(&hadc1, (uint32_t *) rawValues, 2);  
lux = rawValues[0];  // LDR sensor (PA0)  
pot = rawValues[1];  // Potentiometer (PA1)  
sprintf(msg, "Light: %hu     POT: %hu\r\n", lux, pot);  
HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);  

---

## Result

Example serial output when adjusting the potentiometer and covering the LDR:

```c
Light: 4095     POT: 4095  <-- potentiometer max, LDR covered
Light: 4023     POT: 4095
Light: 4021     POT: 3248  <-- potentiometer lowered
Light: 4001     POT: 2731
Light: 2453     POT: 1221  <-- LDR covered, large drop visible
```

Two clear behaviors emerge:

1. **Potentiometer at max → LDR reading stays high (~4095)**  
   Covering the sensor has little visible effect because the ADC is already near full scale.  
   📌 **Takeaway:** High input voltage leaves little dynamic range for changes.

2. **Potentiometer lowered → LDR reading drops significantly when covered**  
   Covering the LDR reduces its voltage divider output, producing a sharp ADC drop.  
   📌 **Takeaway:** Lowering the divider shifts the sensor into a more responsive range.

---

## Hardware

- STM32F303RE Nucleo board  
- Breadboard  
- LDR (light sensor)  
- 10kΩ resistor (for LDR voltage divider)  
- 10kΩ potentiometer  
- Jumper wires  

---

## Testing

1. Flash the program using **STM32CubeIDE**.  
2. Open a serial terminal (PuTTY, TeraTerm, etc.)  
   - Baud: **38400**  
   - COM port: auto-detected via ST-Link  
3. Observe live readings:  
   - Rotate the potentiometer to adjust the `POT` value.  
   - Cover/uncover the LDR to see `Light` values change.  
   - Note how potentiometer setting influences LDR sensitivity.  

---

## Takeaways

- Demonstrates **multi-channel ADC scanning with DMA**.  
- Efficiently streams analog values without blocking CPU.  
- Shows how **voltage divider math + ADC resolution** affect sensor responsiveness.  
- Reinforces UART-based debugging for real-time embedded development.  

---

