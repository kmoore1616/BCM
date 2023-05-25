# The purpose of this program is to take the output txt file from the flight
# computer and convert it into a group of lists and Graphs.
import matplotlib.pyplot as plt

line_on = 0
word_on = 0
time = []
Temp = []
Humidity = []
Pressure = []
Altitude = []
eCO2 = []
TVOC = []

data_file = open("DATA.txt", "r")
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
        
        word_on+=1
    elif word_on == 2:
        Temp.append(float(item))
        word_on+=1
    elif word_on == 3:
        Humidity.append(float(item))
        word_on+=1
    elif word_on == 4:
        
        word_on+=1
    elif word_on == 5:
        Pressure.append(float(item))
        word_on += 1
    elif word_on == 6:
        Altitude.append(float(item))
        word_on += 1
    elif word_on == 7:
        eCO2.append(float(item))
        word_on += 1
    elif word_on == 8:
        TVOC.append(float(item))
        word_on = 0
        
data_file.close()

for data in range(len(time)):
    time[data] = float(time[data])

    
for data in range(len(Temp)):
    Temp[data] = float(Temp[data])
    
for data in range(len(Humidity)):
    Humidity[data] = float(Humidity[data])
    


for data in range(len(Pressure)):
    Pressure[data] = float(Pressure[data])
    
for data in range(len(Altitude)):
    Altitude[data] = float(Altitude[data])
    
for data in range(len(eCO2)):
    eCO2[data] = float(eCO2[data])

for data in range(len(TVOC)):
    TVOC[data] = float(TVOC[data])


figure, axis = plt.subplots(4, 2)


axis[0,0].plot(time,Temp)
axis[0,0].set_title("Main Temp")

axis[0,1].plot(time,Humidity)
axis[0,1].set_title("Humidity")

axis[1,0].plot(time, Pressure)
axis[1,0].set_title("Pressure")

axis[2,0].plot(time, Altitude)
axis[2,0].set_title("Altitude")

axis[2,1].plot(time, eCO2)
axis[2,1].set_title("eCO2")

axis[3,0].plot(time, TVOC)
axis[3,0].set_title("TVOC")
plt.show()


