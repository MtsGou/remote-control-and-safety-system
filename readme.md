Codes containing development of remotely controlled robot and systems prototypes for autonomous vehicles technology, including emergency and security intelligence.

The Emergency and Monitoring System is responsible for managing the actions to be taken in emergency situations, caused by general, hardware or software failures. The system consists of an electronic board with embedded application, using microcontrollers STM32L476RG from ST Microelectronics, whose processor is the ARM model 32-bit Cortex® M4, and ESP32, widely used in IoT applications, whose processor is the 32-bit Xtensa® Dual-Core LX6.

Under normal circumstances, this system takes no action andworks passively, without influencing navigation or navigation in any way.

The system is active in situations that it deems to be emergent, in terms of hardware and software, or if there is direct order from the processing unit or the controller. Also has output pins whose signals are transmitted directly to the truck's electronic control units, the ECMs, exercising the drive-by-wire function. Its signals are responsible for acting only in emergency cases, sending the necessary information to the brakes (emergency and delay) and to the accelerator, so that the truck brakes appropriately. The activation is done by relays, which, in normal position, select the UPA signals for the drive-by-wire outputs. When it becomes active, the system switches the relays, selecting for the outputs the signals generated by the microcontrollers or, in other cases, by oscillators (circuit integrated NE555).

<p align="center">
<img src="./pic/gif_autonomous.gif" />
</p>

## ESP32 MCU

[..]
    
    PINOUT:

    (#) CPU communication PINS:
      (*) IN_EXTRA_CPU: Receive commands from the CPU to brake or disable brake.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) OUT_EXTRA_CPU: Send feedback to the CPU after action.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) IN_PWM_CPU: Receives the PWM indicating the operating status of the CPU.

    (#) UPA communication PINS:
      (*) IN_PWM_UPA: Receives the PWM indicating the UPA's operating status.

    (#) STM communication PINS:
      (*) IO_OUT_ESP_1: Receiving feedback from the STM after its action.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) IO_OUT_ESP_2: Send feedback to STM after action.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) OUT_PWM_ESP: Receives the PWM indicating the operating status of the STM.
      (*) IN_PWM_ESP: Sends a PWM indicating the operating status of the ESP.
      (*) CONNECT_ESP_IN: Sends a digital signal to inform the ESP connection.
                          1 -> connected. 0-> not connected.
      (*) ESP_IN_CLIENTE: Sends a digital signal indicating customer connection.
                        1 -> at least 1 client connected. 0-> not connected.

    (#) Action pins:
      (*) RELE_FREIO_ESP: Digital output responsible for actuating the brake relay.
      (*) FE_ESP: Generation of the emergency brake PWM for the truck's ECM.
      (*) AC_ESP: Generation of the PWM from the accelerator to the truck's ECM.

    (#) SUMMARY:
        (+) Under normal conditions, the relay is not activated.
        (+) PWM reception indicating operating status:
          - Made using external interrupt pins.
          - The interruption resets counters.
          - If the counters exceed a limit,
            and understood as failure.
        (+) Operating conditions:
          - With STM failure, or
          - With direct order from the operator (customer), or
          - With direct order from the CPU, or
          - By dispatch, upon receiving command in TCP protocol
        (+) During actuation, the relay actuates and sends the generated PWM to the ECM.
        (+) A configuration menu can be accessed
            at startup (only).
          - The configured parameters can be saved in the flash.
          - The configured parameters can be erased from the flash.
          - Flash is manipulated using the EEPROM library.
        (+) Events are informed at all times via UART,
            containing timestamp.
        (+) Connection:
          - Attempts to connect to a WiFi AP on the saved network.
          - Must be on the same network as the dispatch.
          - The system works even without a network, however, without
            remote braking and remote monitoring capabilities.
          - This software offers two servers.
          - Server 1: port 80. Communicates via socket with front-end.
          - Server 2: port 8080. Communicates with dispatch via TCP.


## STM32 MCU

[..]
    
    PINOUT:

    (#) CPU communication PINS:
      (*) IN_CPU_6: Receive commands from the CPU to brake or disable brake.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) FEEDBACK_CPU: Send feedback to the CPU after action.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) IN_PWM_CPU: Receives the PWM indicating the operating status of the CPU.
      (*) PWM_OUT_CPU: Sends the PWM indicating the operating status of the STM32.
      (*) GATE_FR: Enables delay brake adjustment by the CPU.
      (*) FR_BIT_1: Bit 1 of the delay brake adjustment by the CPU.
      (*) FR_BIT_2: Bit 2 " ".
      (*) FR_BIT_3: Bit 3 " ".
      (*) FR_BIT_4: Bit 4 " ".
      (*) CPU_OUT_A: First pin to report system status to CPU.
      (*) CPU_OUT_B: Second pin " ".
      (*) CPU_OUT_C: Third pin " ".


    (#) UPA communication PINS:
      (*) IN_PWM_UPA: Receives the PWM indicating the UPA's operating status.
      (*) PWM_OUT_UPA: Sends the PWM indicating the operating status of the STM32.

    (#) ESP communication PINS:
      (*) IO_OUT_ESP_2: Receiving feedback from the ESP after its action.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) IO_OUT_ESP_1: Send feedback to ESP after action.
                          1 -> Brake actuated. 0-> Brake not actuated.
      (*) OUT_PWM_ESP: Sends a PWM indicating the operating status of the STM32.
      (*) IN_PWM_ESP: Receives the PWM indicating the operating status of the ESP.
      (*) CONNECT_ESP_IN: Receives a digital signal to inform the ESP connection.
                          1 -> connected. 0-> not connected.
      (*) ESP_IN_CLIENTE: Receives digital signal from ESP indicating customer connection.
                        1 -> at least 1 client connected. 0-> not connected.

    (#) Action pins:
      (*) RELE_FREIO_STM: Digital output responsible for actuating the brake relay.
      (*) FE_STM: Generation of the emergency brake PWM for the truck's ECM.
      (*) AC_STM: Generation of the PWM from the accelerator to the truck's ECM.
      (*) FR_STM: Generation of the delay brake PWM for the truck's ECM.
      (*) ON_OFF_SWITCH: Pin to turn on the MUX.
      (*) A_SWITCH: Pin A to change the MUX position.
      (*) B_SWITCH: Pin B " "

    (#) Indication pins:
      (*) LED_GREEN: Led to indicate RX reception from the radio (UART 3).
      (*) LED_RED: Led to indicate send TX to radio (UART 3).
      (*) CHECK_SOURCE: Indicates the status of the truck’s battery.
      1 -> Battery OK. 0-> Power failure.

    (#) SUMMARY:
        (+) Under normal conditions, the relays are not activated.
        (+) Default MUX position: UPA signals.
        (+) PWM reception indicating operating status:
          - Made using external interrupt pins.
          - The interruption resets counters.
          - If the counters exceed a limit (timeout),
            and understood as failure.
          - A callback occurs every timer 16 overflow (1 ms).
          - In this callback, monitoring and
            fault identification.
          - ESP Failure: +1, UPA Failure: +2, CPU Failure: +5.
            Ex.: Failure number 7 = UPA and CPU failure (5 + 2)

        (+) Operating conditions:
          - UPA failure
          - CPU failure
          - Failure of both
          - Direct order from the controller (emergency button)
          - With direct order from the CPU
        (+) When actuating, the relay actuates and sends the generated PWM to the ECM,
        in conjunction with changing the position of the MUX (STM).
        (+) A configuration menu can be accessed.
          - The configured parameters can be saved in the flash.
          - The configured parameters can be erased from the flash.
        (+) Events are reported at any time via UART4,
            containing timestamp (time after start of operation).
        (+) Connection:
          - Receives PING from the transmitter in the control tower.
          - Responds PONG to the transmitter.
          - If you stop receiving PING, it indicates loss of STM connection.
        (+) The board has a power outage prevention system.
          - If the system loses power from the battery
        of the truck, a backup battery in parallel, protected
        by a diode, starts to power the system, to keep it
        in proper functioning.
          - If the truck's power is lost, a message will be
          sent remotely to the controller in the control tower.
          - Failure of other autonomous components (UPA and CPU)
            the STM32 will brake immediately.


Codes were written in the C and C++ languages, according to the ANSI C/C++ (American National Standards Institute) standard. Additionally, the front-end page, saved in flash with the name “index_html”, was written using HTML, CSS and JavaScript language. The STM32L4 microcontroller firmware was written in C language, using hardware abstraction layer (HAL), in the STM32Cube IDE. The ESP32 firmware was written in C++ on the platform Espressif PlatformIO.
