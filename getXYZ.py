# ADXL345 Python example 
#
# author:  Jonathan Williamson
# license: BSD, see LICENSE.txt included in this package
# 
# This is an example to show you how to use our ADXL345 Python library
# http://shop.pimoroni.com/products/adafruit-triple-axis-accelerometer

from adxl345 import ADXL345
  
adxl345 = ADXL345()
    
axes = adxl345.getAxes(True)
print "%.f" % ( axes['x'] )
print "%.f" % ( axes['y'] )
print "%.f" % ( axes['z'] )
