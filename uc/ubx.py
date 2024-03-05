import serial
from enum import Enum

class RxState(Enum):
  sIdle = 0
  sSync1 = 1
  sSync2 = 2
  sClass = 3
  sId = 4
  sLen1 = 5
  sLen2 = 6
  sPayload = 7
  sCheckA = 8
  sCheckB = 9

def main():
  ser = serial.Serial('/dev/ttyUSB0', 9600)

  state = RxState.sSync2
  while True:
    if ser.in_waiting == 0:
      continue
    b = ser.read()
    match state:
      case RxState.sSync1:
        if b == b'\xb5':
          state = RxState.sSync2
          
      case RxState.sSync2:
        if b == b'\x62':
          state = RxState.sClass
          ck_a = 0
          ck_b = 0
          valid = True
        else:
          state = RxState.sSync1

      case RxState.sClass:
        ck_a = (ck_a + int.from_bytes(b, 'big')) & 0xff
        ck_b = (ck_b + ck_a) & 0xff
        u_class = int.from_bytes(b, 'big')
        state = RxState.sId

      case RxState.sId:
        ck_a = (ck_a + int.from_bytes(b, 'big')) & 0xff
        ck_b = (ck_b + ck_a) & 0xff
        u_id = int.from_bytes(b, 'big')
        state = RxState.sLen1

      case RxState.sLen1:
        ck_a = (ck_a + int.from_bytes(b, 'big')) & 0xff
        ck_b = (ck_b + ck_a) & 0xff
        len_lsb = b
        state = RxState.sLen2

      case RxState.sLen2:
        ck_a = (ck_a + int.from_bytes(b, 'big')) & 0xff
        ck_b = (ck_b + ck_a) & 0xff
        plen = int.from_bytes(b+len_lsb, 'big')
        pdat = b''

        if plen == 0:
          state = RxState.sCheckA
        else:
          pcnt = 0
          state = RxState.sPayload
        
      case RxState.sPayload:
        ck_a = (ck_a + int.from_bytes(b, 'big')) & 0xff
        ck_b = (ck_b + ck_a) & 0xff

        if pcnt >= 8 and pcnt < 12:
          pdat += b
        pcnt += 1
        if pcnt == plen:
          state = RxState.sCheckA

      case RxState.sCheckA:
        if ck_a != int.from_bytes(b, 'big'):
          valid = False
          state = RxState.sSync1
        state = RxState.sCheckB

      case RxState.sCheckB:
        if ck_b != int.from_bytes(b, 'big'):
          valid = False
        if valid:
          if u_class == 13 and u_id == 1:
            print('qerr: ', int.from_bytes(pdat, 'little', signed=True))
        state = RxState.sSync1

if __name__ == '__main__':
  main()
