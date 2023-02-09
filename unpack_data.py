# The purpose of this program is to take the output txt file from the flight
# computer and convert it into a group of lists and Graphs.
import matplotlib.pyplot as plt

line_on = 0
word_on = 0
time = []
x = []
y = []
z = []
therm = []
humidity = []
iTemp = []
pressure = []
co2 = []

data_file = open("C:\\Users\\ewokk\\Documents\\.PP\Python\\balloon\\BCM\\DATA.txt", "r")
data = data_file.read()

data = data.replace('\n', '').split("|")
for i in range(10):
    data.pop(0)
for item in data:
    if item == "":
        pass
    elif word_on == 0:
        print(item)
        time.append(float(item))
        word_on+=1
    elif word_on == 1:
        x.append(float(item))
        word_on+=1
    elif word_on == 2:
        y.append(float(item))
        word_on+=1
    elif word_on == 3:
        z.append(float(item))
        word_on+=1
    elif word_on == 4:
        therm.append(float(item))
        word_on+=1
    elif word_on == 5:
        humidity.append(float(item))
        word_on += 1
    elif word_on == 6:
        iTemp.append(float(item))
        word_on += 1
    elif word_on == 7:
        pressure.append(float(item))
        word_on += 1
    elif word_on == 8:
        co2.append(float(item))
        word_on = 0
        
data_file.close()



figure, axis = plt.subplots(4, 2)

axis[0,0].plot(time, x)
axis[0,0].set_title("X Acceleration")

axis[0,1].plot(time,y)
axis[0,1].set_title("Y Acceleration")

axis[1,0].plot(time,z)
axis[1,0].set_title("Z Acceleration")

axis[1,1].plot(time,therm)
axis[1,1].plot("Temperature")

axis[2,0].plot(time, humidity)
axis[2,0].set_title("Humidity")

axis[2,1].plot(time, iTemp)
axis[2,1].set_title("Internal Temperature")

axis[3,0].plot(time, pressure)
axis[3,0].set_title("Pressure")

axis[3,1].plot(time, co2)
axis[3,1].set_title("co2")
plt.show()


