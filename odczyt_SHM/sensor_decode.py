import math


# raw data w wysokość
def decode_pressure_sensor(raw_value, base_value):
 
    R = 8.31446261815324;   #stała gazowa
    mi = 0.0289644;         #masa molowa powietrza
    g = 9.8;                #przyspieszenie ziemskie
    T = 293.00;             #temperatura powietrza [K]
    
    p = raw_value / 256.0  
    if(p>0.0):
      
        h=0
        h = (- ((T * R)/(mi * g)* math.log(p/base_value)))
    else:
        h=0
    return h


# poziom baterii
def decode_battery_level(raw_value):
    
    battery_level = raw_value / 65536
    return battery_level * 100.0

# temperatura w [K]
def decode_motor_temperature(raw_value):
    ratio = raw_value / 0xFFFF
    if ratio >= 1.0:
        ratio = 0.999999  # unikamy dzielenia przez 0
    

    NTC_R = 100.0 * ratio / (1.0 - ratio)
    if(NTC_R>0):
        
        temp = 1.0 / (1.0 / 298.15 + math.log(NTC_R / 10000.0) / 3950.0)
        return temp
    else:
        return 0.0
