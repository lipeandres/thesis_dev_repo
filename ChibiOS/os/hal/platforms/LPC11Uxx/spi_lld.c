/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    LPC11Uxx/spi_lld.c
 * @brief   LPC11Uxx low level SPI driver code.
 *
 * @addtogroup SPI
 * @{
 */

#include "ch.h"
#include "hal.h"

#if HAL_USE_SPI || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

#if LPC_SPI_USE_SSP0 || defined(__DOXYGEN__)
/** @brief SPI1 driver identifier.*/
SPIDriver SPID1;
#endif

#if LPC_SPI_USE_SSP1 || defined(__DOXYGEN__)
/** @brief SPI2 driver identifier.*/
SPIDriver SPID2;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Preloads the transmit FIFO.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 */
static void ssp_fifo_preload(SPIDriver *spip) {
  LPC_SSPx_Type *ssp = spip->ssp;
  uint32_t n = spip->txcnt > LPC_SSP_FIFO_DEPTH ?
               LPC_SSP_FIFO_DEPTH : spip->txcnt;

  while(((ssp->SR & SR_TNF) != 0) && (n > 0)) {
    if (spip->txptr != NULL) {
      if ((ssp->CR0 & CR0_DSSMASK) > CR0_DSS8BIT) {
        const uint16_t *p = spip->txptr;
        ssp->DR = *p++;
        spip->txptr = p;
      }
      else {
        const uint8_t *p = spip->txptr;
        ssp->DR = *p++;
        spip->txptr = p;
      }
    }
    else
      ssp->DR = 0xFFFFFFFF;
    n--;
    spip->txcnt--;
  }
}

/**
 * @brief   Common IRQ handler.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 */
static void spi_serve_interrupt(SPIDriver *spip) {
  LPC_SSPx_Type *ssp = spip->ssp;

  if ((ssp->MIS & MIS_ROR) != 0) {
    /* The overflow condition should never happen because priority is given
       to receive but a hook macro is provided anyway...*/
    LPC_SPI_SSP_ERROR_HOOK(spip);
  }
  ssp->ICR = ICR_RT | ICR_ROR;
  while ((ssp->SR & SR_RNE) != 0) {
    if (spip->rxptr != NULL) {
      if ((ssp->CR0 & CR0_DSSMASK) > CR0_DSS8BIT) {
        uint16_t *p = spip->rxptr;
        *p++ = ssp->DR;
        spip->rxptr = p;
      }
      else {
        uint8_t *p = spip->rxptr;
        *p++ = ssp->DR;
        spip->rxptr = p;
      }
    }
    else
      (void)ssp->DR;
    if (--spip->rxcnt == 0) {
      chDbgAssert(spip->txcnt == 0,
                  "spi_serve_interrupt(), #1", "counter out of synch");
      /* Stops the IRQ sources.*/
      ssp->IMSC = 0;
      /* Portable SPI ISR code defined in the high level driver, note, it is
         a macro.*/
      _spi_isr_code(spip);
      return;
    }
  }
  ssp_fifo_preload(spip);
  if (spip->txcnt == 0)
    ssp->IMSC = IMSC_ROR | IMSC_RT | IMSC_RX;
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if LPC_SPI_USE_SSP0 || defined(__DOXYGEN__)
/**
 * @brief   SSP0 interrupt handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(Vector90) {

  CH_IRQ_PROLOGUE();

  spi_serve_interrupt(&SPID1);

  CH_IRQ_EPILOGUE();
}
#endif

#if LPC_SPI_USE_SSP1 || defined(__DOXYGEN__)
/**
 * @brief   SSP1 interrupt handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(Vector78) {

  CH_IRQ_PROLOGUE();

  spi_serve_interrupt(&SPID2);

  CH_IRQ_EPILOGUE();
}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level SPI driver initialization.
 *
 * @notapi
 */
void spi_lld_init(void) {

#if LPC_SPI_USE_SSP0
  spiObjectInit(&SPID1);
  SPID1.ssp = LPC_SSP0;
#endif /* LPC_SPI_USE_SSP0 */

#if LPC_SPI_USE_SSP1
  spiObjectInit(&SPID2);
  SPID2.ssp = LPC_SSP1;
#endif /* LPC_SPI_USE_SSP0 */
}

/**
 * @brief   Configures and activates the SPI peripheral.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_start(SPIDriver *spip) {

  if (spip->state == SPI_STOP) {
    /* Clock activation.*/
#if LPC_SPI_USE_SSP0
    if (&SPID1 == spip) {
      LPC_SYSCON->SSP0CLKDIV = LPC_SPI_SSP0CLKDIV;
      LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 11);
      LPC_SYSCON->PRESETCTRL |= 1;
      nvicEnableVector(SSP0_IRQn,
                       CORTEX_PRIORITY_MASK(LPC_SPI_SSP0_IRQ_PRIORITY));
    }
#endif
#if LPC_SPI_USE_SSP1
    if (&SPID2 == spip) {
      LPC_SYSCON->SSP1CLKDIV = LPC_SPI_SSP1CLKDIV;
      LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 18);
      LPC_SYSCON->PRESETCTRL |= 4;
      nvicEnableVector(SSP1_IRQn,
                       CORTEX_PRIORITY_MASK(LPC_SPI_SSP1_IRQ_PRIORITY));
    }
#endif
  }
  /* Configuration.*/
  spip->ssp->CR1  = 0;
  spip->ssp->ICR  = ICR_RT | ICR_ROR;
  spip->ssp->CR0  = spip->config->cr0;
  spip->ssp->CPSR = spip->config->cpsr;
  spip->ssp->CR1  = CR1_SSE;
}

/**
 * @brief   Deactivates the SPI peripheral.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_stop(SPIDriver *spip) {

  if (spip->state != SPI_STOP) {
    spip->ssp->CR1  = 0;
    spip->ssp->CR0  = 0;
    spip->ssp->CPSR = 0;
#if LPC_SPI_USE_SSP0
    if (&SPID1 == spip) {
      LPC_SYSCON->PRESETCTRL &= ~1;
      LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 11);
      LPC_SYSCON->SSP0CLKDIV = 0;
      nvicDisableVector(SSP0_IRQn);
    }
#endif
#if LPC_SPI_USE_SSP1
    if (&SPID2 == spip) {
      LPC_SYSCON->PRESETCTRL &= ~4;
      LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 18);
      LPC_SYSCON->SSP1CLKDIV = 0;
      nvicDisableVector(SSP1_IRQn);
    }
#endif
  }
}

/**
 * @brief   Asserts the slave select signal and prepares for transfers.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_select(SPIDriver *spip) {

  palClearPad(spip->config->ssport, spip->config->sspad);
}

/**
 * @brief   Deasserts the slave select signal.
 * @details The previously selected peripheral is unselected.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 *
 * @notapi
 */
void spi_lld_unselect(SPIDriver *spip) {

  palSetPad(spip->config->ssport, spip->config->sspad);
}

/**
 * @brief   Ignores data on the SPI bus.
 * @details This function transmits a series of idle words on the SPI bus and
 *          ignores the received data. This function can be invoked even
 *          when a slave select signal has not been yet asserted.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to be ignored
 *
 * @notapi
 */
void spi_lld_ignore(SPIDriver *spip, size_t n) {

  spip->rxptr = NULL;
  spip->txptr = NULL;
  spip->rxcnt = spip->txcnt = n;
  ssp_fifo_preload(spip);
  spip->ssp->IMSC = IMSC_ROR | IMSC_RT | IMSC_TX | IMSC_RX;
}

/**
 * @brief   Exchanges data on the SPI bus.
 * @details This asynchronous function starts a simultaneous transmit/receive
 *          operation.
 * @post    At the end of the operation the configured callback is invoked.
 * @note    The buffers are organized as uint8_t arrays for data sizes below or
 *          equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to be exchanged
 * @param[in] txbuf     the pointer to the transmit buffer
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @notapi
 */
void spi_lld_exchange(SPIDriver *spip, size_t n,
                      const void *txbuf, void *rxbuf) {

  spip->rxptr = rxbuf;
  spip->txptr = txbuf;
  spip->rxcnt = spip->txcnt = n;
  ssp_fifo_preload(spip);
  spip->ssp->IMSC = IMSC_ROR | IMSC_RT | IMSC_TX | IMSC_RX;
}

/**
 * @brief   Sends data over the SPI bus.
 * @details This asynchronous function starts a transmit operation.
 * @post    At the end of the operation the configured callback is invoked.
 * @note    The buffers are organized as uint8_t arrays for data sizes below or
 *          equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to send
 * @param[in] txbuf     the pointer to the transmit buffer
 *
 * @notapi
 */
void spi_lld_send(SPIDriver *spip, size_t n, const void *txbuf) {

  spip->rxptr = NULL;
  spip->txptr = txbuf;
  spip->rxcnt = spip->txcnt = n;
  ssp_fifo_preload(spip);
  spip->ssp->IMSC = IMSC_ROR | IMSC_RT | IMSC_TX | IMSC_RX;
}

/**
 * @brief   Receives data from the SPI bus.
 * @details This asynchronous function starts a receive operation.
 * @post    At the end of the operation the configured callback is invoked.
 * @note    The buffers are organized as uint8_t arrays for data sizes below or
 *          equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] n         number of words to receive
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @notapi
 */
void spi_lld_receive(SPIDriver *spip, size_t n, void *rxbuf) {

  spip->rxptr = rxbuf;
  spip->txptr = NULL;
  spip->rxcnt = spip->txcnt = n;
  ssp_fifo_preload(spip);
  spip->ssp->IMSC = IMSC_ROR | IMSC_RT | IMSC_TX | IMSC_RX;
}

/**
 * @brief   Exchanges one frame using a polled wait.
 * @details This synchronous function exchanges one frame using a polled
 *          synchronization method. This function is useful when exchanging
 *          small amount of data on high speed channels, usually in this
 *          situation is much more efficient just wait for completion using
 *          polling than suspending the thread waiting for an interrupt.
 *
 * @param[in] spip      pointer to the @p SPIDriver object
 * @param[in] frame     the data frame to send over the SPI bus
 * @return              The received data frame from the SPI bus.
 */
uint16_t spi_lld_polled_exchange(SPIDriver *spip, uint16_t frame) {

  spip->ssp->DR = (uint32_t)frame;
  while ((spip->ssp->SR & SR_RNE) == 0)
    ;
  return (uint16_t)spip->ssp->DR;
}

#endif /* HAL_USE_SPI */

/** @} */
