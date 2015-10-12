Overview:

1.Data arrives from external world (Say through USB Serial COM port on PC to
GPIO pins on USART port, using RealTerm on Windows PC to send and receive data).

2: Peripheral sends, request signal to DMA controller. This leads to Interrupt
request (DMA_IRQ) generation.(H/W)

3.DMA controller Acknowldeges the requesting Peripheral. (H/W)

4.ISR associated with the DMA_IRQ (DMA1_Channel2_3_IRQHandler) gets invoked. 
Polling for 'transfer complete flag' starts.(S/W)

5.Bus Matrix, ask Cortex Core to relinquish the System Bus. 

6.Core relinquishes the System bus, Bus Matrix allows DMA controller to access
the system bus. (H/W)

7.DMA controller access the Peripheral register. Data is transferred from
peripheral (USART1) to memory. (H/W)

8.Data transfer happens over a pre-configured channel(DMA1, Channel 3). The
channel selection is done according to the target device(STM32F051xx) data sheet
and reference manual. (H/W)

9. Bus matrix does a round robin scheduling of granting bus access between the
DMA controller and CPU Core. Once the data transfer (As configured during the
DMA setup, such as data size, memory location, direction etc) is complete,
DMA controller relinquish the bus. (H/W)

10.Bus Matrix grants, System bus access to CPU. (H/W) 

11.DMA controller sets 'Transfer complete flag' for Systems programming
purposes(H/W)

12.In the DMA ISR, this transfer complete flag is checked and once set, 
necessary action (toggling the LED on board) is taken and before leaving ISR,
'transfer complete flag' is reset.(S/W)

Notation:
H/W = Handled by Hardware
S/W = Handled by Software
