# STM32 Multi-Channel ADC with DMA (LDR + Potentiometer)

This project uses the STM32F303RE Nucleo board to read two analog inputs - a light-dependent resistor (LDR) and a 10kΩ potentiometer, using **DMA and interrupt-based ADC**. The readings are printed to a serial terminal via UART, showing how both signals change in real time.

It demonstrates how to:
- Configure **multi-channel ADC** with **DMA**
- Format and transmit sensor data over **UART2**

---

## 🔧 Hardware

- STM32F303RE Nucleo board  
- Breadboard  
- LDR (light sensor)  
- 10kΩ resistor (for LDR voltage divider)  
- 10kΩ potentiometer  
- Jumper wires

### Circuit Diagram

```
          LDR Voltage Divider (PA0)
          --------------------------
             3.3V
              │
            [ LDR ]
              │────── PA0 (ADC1_CH1)
              │
            [10kΩ]
              │
             GND


     Potentiometer Divider (PA1)
     ---------------------------
     GND ─────┬───────┐
              │       ▼
            [ POT ] <─── 5V
              │
             PA1 (ADC1_CH2)
```

- The **LDR** and fixed resistor form a voltage divider; the middle node is connected to **PA0**.
- The **potentiometer** sweeps voltage between 0V and 5V, connected to **PA1**.
- UART2 (PA2) is used for serial output.

---

## ⚙️ ADC Configuration

- **ADC1**: scan mode with 2 channels
  - **Channel 0 (PA0)**: LDR
  - **Channel 1 (PA1)**: Potentiometer
- **DMA**: circular mode, 2-value buffer (`rawValues[2]`)
- **Interrupts**: ADC triggers `HAL_ADC_ConvCpltCallback()` when both values are ready
- **UART2**: 115200 baud, prints live sensor readings

---

## 📟 Live Output Example

As the potentiometer is adjusted and the LDR is exposed or covered, this is what the serial monitor shows:

```
Light: 4095     POT: 4095  <-- sensor covered
Light: 4023     POT: 4095
Light: 4021     POT: 3248  <-- starting to decrease the potentiometer
Light: 4001     POT: 2731
Light: 4026     POT: 2435
Light: 4020     POT: 1209
Light: 3884     POT: 1730
Light: 2453     POT: 1221  
Light: 1242     POT: 1223  <-- sensor still covered
```

---

## 🎯 Two Key Observations

### **1) Potentiometer is at max → covering the LDR does *not* visibly change the reading**

When the potentiometer is at its maximum (5V), the LDR reading stays pinned near 4095 even when you cover the sensor with your finger. This happens because the voltage across the LDR is already close to the ADC’s full scale, and covering the sensor doesn't reduce the light enough to cause a large voltage swing.

📌 **Why this matters:** A high ADC value leaves little room for downward change. Unless the light drops dramatically, the change might be too small to register meaningfully.

---

### **2) Potentiometer is turned down → now covering the LDR causes a clear drop**

Once the potentiometer is lowered, the LDR reading responds much more noticeably to changes in light. As you cover the sensor, the value drops quickly from ~4000 to below 1500.

📌 **Why this matters:** This makes the ADC input more sensitive to changes in the voltage divider, allowing the LDR’s behavior to show more clearly.

---

## 🧠 Why This Happens

This is a great real-world illustration of how **analog voltage dividers** and **ADC resolution** work together:

- The LDR is part of a voltage divider. Its resistance increases in darkness.
- But the output voltage (what the ADC reads) depends on the ratio between the LDR and the fixed resistor.
- When starting near 3.3V or 5V, the voltage doesn't shift much unless the LDR changes drastically.
- Lowering the potentiometer lets you "zoom in" on the LDR’s dynamic range.

---

## 🧰 Code Highlights

```c
// DMA buffer for both ADC channels
uint16_t rawValues[2];

// ADC conversion complete interrupt
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    convCompleted = 1;
}

// In the main loop
lux = rawValues[0];  // LDR value
pot = rawValues[1];  // Potentiometer value

// Print both readings to serial monitor
sprintf(msg, "Light: %hu     POT: %hu\r\n", lux, pot);
HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
```

---

## ✅ Testing

1. Upload the code via **STM32CubeIDE**.
2. Open **PuTTY** or any serial terminal.
   - **Baud rate**: `38400`
   - **COM port**: Auto-detected (check Device Manager)
3. Observe live readings printed to the terminal.
4. Rotate the potentiometer slowly and watch the `POT` value change.
5. While the potentiometer is:
   - **At max** → cover the LDR → `Light` value stays ~4095
   - **Turned down** → cover the LDR → `Light` value drops significantly

---

## 🧠 Skills Demonstrated

- STM32 peripheral configuration
- DMA-driven ADC scan with interrupt callbacks
- UART-based live debugging
- Analog circuit understanding

---

