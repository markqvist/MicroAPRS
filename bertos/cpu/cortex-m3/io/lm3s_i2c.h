/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2010 Develer S.r.l. (http:///<www.develer.com/)
 *
 * -->
 *
 * \brief LM3S I2C definition.
 */

#ifndef LM3S_I2C_H
#define LM3S_I2C_H


/* The following are defines for the I2C register offsets. */
#define I2C_O_MSA               0x00000000  ///< I2C Master Slave Address
#define I2C_O_SOAR              0x00000000  ///< I2C Slave Own Address
#define I2C_O_SCSR              0x00000004  ///< I2C Slave Control/Status
#define I2C_O_MCS               0x00000004  ///< I2C Master Control/Status
#define I2C_O_SDR               0x00000008  ///< I2C Slave Data
#define I2C_O_MDR               0x00000008  ///< I2C Master Data
#define I2C_O_MTPR              0x0000000C  ///< I2C Master Timer Period
#define I2C_O_SIMR              0x0000000C  ///< I2C Slave Interrupt Mask
#define I2C_O_SRIS              0x00000010  ///< I2C Slave Raw Interrupt Status
#define I2C_O_MIMR              0x00000010  ///< I2C Master Interrupt Mask
#define I2C_O_MRIS              0x00000014  ///< I2C Master Raw Interrupt Status
#define I2C_O_SMIS              0x00000014  ///< I2C Slave Masked Interrupt
                                            ///< Status
#define I2C_O_SICR              0x00000018  ///< I2C Slave Interrupt Clear
#define I2C_O_MMIS              0x00000018  ///< I2C Master Masked Interrupt
                                            ///< Status
#define I2C_O_MICR              0x0000001C  ///< I2C Master Interrupt Clear
#define I2C_O_MCR               0x00000020  ///< I2C Master Configuration


/* The following are defines for the bit fields in the I2C_O_MSA register. */
#define I2C_MSA_SA_M            0x000000FE  ///< I2C Slave Address
#define I2C_MSA_RS              0x00000001  ///< Receive not send
#define I2C_MSA_SA_S            1
#define I2C_MSA_ADDS            0           ///< Set address write bit
#define I2C_MSA_ADDR            1           ///< Set address read bit


/* The following are defines for the bit fields in the I2C_O_SOAR register. */
#define I2C_SOAR_OAR_M          0x0000007F  ///< I2C Slave Own Address
#define I2C_SOAR_OAR_S          0


/* The following are defines for the bit fields in the I2C_O_SCSR register. */
#define I2C_SCSR_FBR            0x00000004  ///< First Byte Received
#define I2C_SCSR_TREQ           0x00000002  ///< Transmit Request
#define I2C_SCSR_DA             0x00000001  ///< Device Active
#define I2C_SCSR_RREQ           0x00000001  ///< Receive Request

/* The following are defines for the bit fields in the I2C_O_MCS register. */
#define I2C_MCS_BUSBSY          0x00000040  ///< Bus Busy
#define I2C_MCS_IDLE            0x00000020  ///< I2C Idle
#define I2C_MCS_ARBLST          0x00000010  ///< Arbitration Lost
#define I2C_MCS_ACK             0x00000008  ///< Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  ///< Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  ///< Acknowledge Address
#define I2C_MCS_STOP            0x00000004  ///< Generate STOP
#define I2C_MCS_START           0x00000002  ///< Generate START
#define I2C_MCS_ERROR           0x00000002  ///< Error
#define I2C_MCS_RUN             0x00000001  ///< I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  ///< I2C Busy


/* The following are defines for the bit fields in the I2C_O_SDR register. */
#define I2C_SDR_DATA_M          0x000000FF  ///< Data for Transfer
#define I2C_SDR_DATA_S          0

/* The following are defines for the bit fields in the I2C_O_MDR register. */
#define I2C_MDR_DATA_M          0x000000FF  ///< Data Transferred
#define I2C_MDR_DATA_S          0


/* The following are defines for the bit fields in the I2C_O_MTPR register. */
#define I2C_MTPR_TPR_M          0x000000FF  ///< SCL Clock Period
#define I2C_MTPR_TPR_S          0


/* The following are defines for the bit fields in the I2C_O_SIMR register. */
#define I2C_SIMR_STOPIM         0x00000004  ///< Stop Condition Interrupt Mask
#define I2C_SIMR_STARTIM        0x00000002  ///< Start Condition Interrupt Mask
#define I2C_SIMR_DATAIM         0x00000001  ///< Data Interrupt Mask


/* The following are defines for the bit fields in the I2C_O_SRIS register. */
#define I2C_SRIS_STOPRIS        0x00000004  ///< Stop Condition Raw Interrupt
                                            ///< Status
#define I2C_SRIS_STARTRIS       0x00000002  ///< Start Condition Raw Interrupt
                                            ///< Status
#define I2C_SRIS_DATARIS        0x00000001  ///< Data Raw Interrupt Status


/* The following are defines for the bit fields in the I2C_O_MIMR register. */
#define I2C_MIMR_IM             0x00000001  ///< Interrupt Mask


/* The following are defines for the bit fields in the I2C_O_MRIS register. */
#define I2C_MRIS_RIS            0x00000001  ///< Raw Interrupt Status

/* The following are defines for the bit fields in the I2C_O_SMIS register. */
#define I2C_SMIS_STOPMIS        0x00000004  ///< Stop Condition Masked Interrupt
                                            ///< Status
#define I2C_SMIS_STARTMIS       0x00000002  ///< Start Condition Masked Interrupt
                                            ///< Status
#define I2C_SMIS_DATAMIS        0x00000001  ///< Data Masked Interrupt Status

/* The following are defines for the bit fields in the I2C_O_SICR register. */
#define I2C_SICR_STOPIC         0x00000004  ///< Stop Condition Interrupt Clear
#define I2C_SICR_STARTIC        0x00000002  ///< Start Condition Interrupt Clear
#define I2C_SICR_DATAIC         0x00000001  ///< Data Interrupt Clear

/* The following are defines for the bit fields in the I2C_O_MMIS register. */
#define I2C_MMIS_MIS            0x00000001  ///< Masked Interrupt Status


/* The following are defines for the bit fields in the I2C_O_MICR register. */
#define I2C_MICR_IC             0x00000001  ///< Interrupt Clear


/* The following are defines for the bit fields in the I2C_O_MCR register. */
#define I2C_MCR_SFE             0x00000020  ///< I2C Slave Function Enable
#define I2C_MCR_MFE             0x00000010  ///< I2C Master Function Enable
#define I2C_MCR_LPBK            0x00000001  ///< I2C Loopback



#define I2C_MASTER_CMD_SINGLE_SEND                     0x00000007
#define I2C_MASTER_CMD_SINGLE_RECEIVE                  0x00000007

#define I2C_MASTER_CMD_BURST_SEND_START                0x00000003
#define I2C_MASTER_CMD_BURST_SEND_CONT                 0x00000001
#define I2C_MASTER_CMD_BURST_SEND_FINISH               0x00000005
#define I2C_MASTER_CMD_BURST_SEND_ERROR_STOP           0x00000004

#define I2C_MASTER_CMD_BURST_RECEIVE_START             0x0000000b
#define I2C_MASTER_CMD_BURST_RECEIVE_CONT              0x00000009
#define I2C_MASTER_CMD_BURST_RECEIVE_FINISH            0x00000005
#define I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP        0x00000004


/* The following definitions are deprecated. */

#ifndef DEPRECATED

	#define I2C_O_SLAVE             0x00000800  ///< Offset from master to slave

	#define I2C_SIMR_IM             0x00000001  ///< Interrupt Mask

	#define I2C_SRIS_RIS            0x00000001  ///< Raw Interrupt Status

	#define I2C_SMIS_MIS            0x00000001  ///< Masked Interrupt Status

	#define I2C_SICR_IC             0x00000001  ///< Clear Interrupt

	#define I2C_MASTER_O_SA         0x00000000  ///< Slave address register
	#define I2C_MASTER_O_CS         0x00000004  ///< Control and Status register
	#define I2C_MASTER_O_DR         0x00000008  ///< Data register
	#define I2C_MASTER_O_TPR        0x0000000C  ///< Timer period register
	#define I2C_MASTER_O_IMR        0x00000010  ///< Interrupt mask register
	#define I2C_MASTER_O_RIS        0x00000014  ///< Raw interrupt status register
	#define I2C_MASTER_O_MIS        0x00000018  ///< Masked interrupt status reg
	#define I2C_MASTER_O_MICR       0x0000001C  ///< Interrupt clear register
	#define I2C_MASTER_O_CR         0x00000020  ///< Configuration register

	#define I2C_SLAVE_O_SICR        0x00000018  ///< Interrupt clear register
	#define I2C_SLAVE_O_MIS         0x00000014  ///< Masked interrupt status reg
	#define I2C_SLAVE_O_RIS         0x00000010  ///< Raw interrupt status register
	#define I2C_SLAVE_O_IM          0x0000000C  ///< Interrupt mask register
	#define I2C_SLAVE_O_DR          0x00000008  ///< Data register
	#define I2C_SLAVE_O_CSR         0x00000004  ///< Control/Status register
	#define I2C_SLAVE_O_OAR         0x00000000  ///< Own address register

	#define I2C_MASTER_SA_SA_MASK   0x000000FE  ///< Slave address
	#define I2C_MASTER_SA_RS        0x00000001  ///< Receive/send
	#define I2C_MASTER_SA_SA_SHIFT  1

	#define I2C_MASTER_CS_BUS_BUSY  0x00000040  ///< Bus busy
	#define I2C_MASTER_CS_IDLE      0x00000020  ///< Idle
	#define I2C_MASTER_CS_ERR_MASK  0x0000001C
	#define I2C_MASTER_CS_BUSY      0x00000001  ///< Controller is TX/RX data
	#define I2C_MASTER_CS_ERROR     0x00000002  ///< Error occurred
	#define I2C_MASTER_CS_ADDR_ACK  0x00000004  ///< Address byte not acknowledged
	#define I2C_MASTER_CS_DATA_ACK  0x00000008  ///< Data byte not acknowledged
	#define I2C_MASTER_CS_ARB_LOST  0x00000010  ///< Lost arbitration
	#define I2C_MASTER_CS_ACK       0x00000008  ///< Acknowlegde
	#define I2C_MASTER_CS_STOP      0x00000004  ///< Stop
	#define I2C_MASTER_CS_START     0x00000002  ///< Start
	#define I2C_MASTER_CS_RUN       0x00000001  ///< Run


	#define I2C_SCL_FAST            400000      ///< SCL fast frequency
	#define I2C_SCL_STANDARD        100000      ///< SCL standard frequency
	#define I2C_MASTER_TPR_SCL_LP   0x00000006  ///< SCL low period
	#define I2C_MASTER_TPR_SCL_HP   0x00000004  ///< SCL high period
	#define I2C_MASTER_TPR_SCL      (I2C_MASTER_TPR_SCL_HP + I2C_MASTER_TPR_SCL_LP)

	#define I2C_MASTER_IMR_IM       0x00000001  ///< Master interrupt mask

	#define I2C_MASTER_RIS_RIS      0x00000001  ///< Master raw interrupt status

	#define I2C_MASTER_MIS_MIS      0x00000001  ///< Master masked interrupt status

	#define I2C_MASTER_MICR_IC      0x00000001  ///< Master interrupt clear

	#define I2C_MASTER_CR_SFE       0x00000020  ///< Slave function enable
	#define I2C_MASTER_CR_MFE       0x00000010  ///< Master function enable
	#define I2C_MASTER_CR_LPBK      0x00000001  ///< Loopback enable

	#define I2C_SLAVE_SOAR_OAR_MASK 0x0000007F  ///< Slave address

	#define I2C_SLAVE_CSR_FBR       0x00000004  ///< First byte received from master
	#define I2C_SLAVE_CSR_TREQ      0x00000002  ///< Transmit request received
	#define I2C_SLAVE_CSR_DA        0x00000001  ///< Enable the device
	#define I2C_SLAVE_CSR_RREQ      0x00000001  ///< Receive data from I2C master

	#define I2C_SLAVE_IMR_IM        0x00000001  ///< Slave interrupt mask

	#define I2C_SLAVE_RIS_RIS       0x00000001  ///< Slave raw interrupt status

	#define I2C_SLAVE_MIS_MIS       0x00000001  ///< Slave masked interrupt status

	#define I2C_SLAVE_SICR_IC       0x00000001  ///< Slave interrupt clear

#endif

#endif /* LM3S_I2C_H */
