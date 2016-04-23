//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <boost/container/set.hpp>
#include <unistd.h>
#include <stdio.h>
#include "usblink.h"
#include "pixy.h"
#include "utils/timer.hpp"
#include "debuglog.h"

boost::container::set<uint8_t> USBLink::devices_in_use_;

USBLink::USBLink()
{
  open_ = false;
  device_address_ = 0;
  m_handle = 0;
  m_context = 0;
  m_blockSize = 64;
  m_flags = LINK_FLAG_ERROR_CORRECTED;
}

USBLink::~USBLink()
{
  close();
}

int USBLink::open()
{
  close();

  libusb_init(&m_context);

  return openDevice();
}

void USBLink::close()
{
  if (m_handle)
  {
    libusb_close(m_handle);
    m_handle = 0;
  }
  if (m_context)
  {
    libusb_exit(m_context);
    m_context = 0;
  }
  if (open_) {
    devices_in_use_.erase(device_address_);
  }
  device_address_ = 0;
  open_ = false;
}

int USBLink::numDevices()
{
  libusb_device **list = NULL;
  int count = 0;
  int num_pixies = 0;
  libusb_device *device;
  libusb_device_descriptor desc;
  libusb_context *context;

  libusb_init(&context);
  count = libusb_get_device_list(context, &list);
  for (int i = 0; i < count; i++)
  {
    device = list[i];
    libusb_get_device_descriptor(device, &desc);
    if (desc.idVendor==PIXY_VID && desc.idProduct==PIXY_PID)
      num_pixies++;
  }
  libusb_free_device_list(list, 1);
  return num_pixies;
}

int USBLink::openDevice()
{
  libusb_device **list = NULL;
  int i, count = 0;
  libusb_device *device;
  libusb_device_descriptor desc;

#ifdef __MACOS__
  const unsigned int MILLISECONDS_TO_SLEEP = 100;
#endif

  count = libusb_get_device_list(m_context, &list);

  for (i=0; i<count; i++)
  {
    device = list[i];
    libusb_get_device_descriptor(device, &desc);
    const uint8_t device_address = libusb_get_device_address(device);

    if (desc.idVendor==PIXY_VID && desc.idProduct==PIXY_PID)
    {
      if (libusb_open(device, &m_handle)==0)
      {
#ifdef __MACOS__
        libusb_reset_device(m_handle);
        usleep(MILLISECONDS_TO_SLEEP * 1000);
#endif
        if ((devices_in_use_.find(device_address) != devices_in_use_.cend()) ||
            (libusb_set_configuration(m_handle, 1) < 0) ||
            (libusb_claim_interface(m_handle, 1) < 0)) {
          libusb_close(m_handle);
          m_handle = 0;
          continue;
        }
#ifdef __LINUX__
        libusb_reset_device(m_handle);
#endif
        devices_in_use_.insert(device_address);
        device_address_ = device_address;
        open_ = true;
        break;
      }
    }
  }
  libusb_free_device_list(list, 1);
  if (i==count) // no devices found
    return -1;
  return 0;
}



int USBLink::send(const uint8_t *data, uint32_t len, uint16_t timeoutMs)
{
  int res, transferred;

  if (timeoutMs==0) // 0 equals infinity
    timeoutMs = 10;

  if ((res=libusb_bulk_transfer(m_handle, 0x02, (unsigned char *)data, len, &transferred, timeoutMs))<0)
  {
#ifdef __MACOS__
    libusb_clear_halt(m_handle, 0x02);
#endif
    log("pixydebug: USBLink::send() returned %d\n", res);
    return res;
  }
  
  log("pixydebug: USBLink::send() returned %d\n", transferred);
  return transferred;
}

int USBLink::receive(uint8_t *data, uint32_t len, uint16_t timeoutMs)
{
  int res, transferred;

  if (timeoutMs==0) // 0 equals infinity
    timeoutMs = 50;

  // Note: if this call is taking more time than than expected, check to see if we're connected as USB 2.0.  Bad USB cables can
  // cause us to revert to a 1.0 connection.
  if ((res=libusb_bulk_transfer(m_handle, 0x82, (unsigned char *)data, len, &transferred, timeoutMs))<0)
  {
    log("pixydebug: libusb_bulk_transfer() = %d\n", res);
#ifdef __MACOS__
    libusb_clear_halt(m_handle, 0x82);
#endif
    return res;
  }
  return transferred;
}

void USBLink::setTimer()
{
  timer_.reset();
}

uint32_t USBLink::getTimer()
{
  return timer_.elapsed();
}
