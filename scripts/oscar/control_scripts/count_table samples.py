import matplotlib.pyplot as plt
ff = open("calibration_table.pb.txt", 'r')
k=0
speed = []
acc = []
for line in ff:
    try:
        if line.split()[0] == 'calibration':
            k+=1
        if line.split()[0] == 'speed:':
            speed.append(float(line.split()[1]))
        if line.split()[0] == 'command:':
            acc.append(float(line.split()[1]))
    except:
        pass
print("Samples: ", k)
plt.figure(figsize=(20,10))
plt.scatter(speed, acc)
plt.xlabel("Speed, m/s")
plt.ylabel("Commands, %")
plt.savefig("speed-acc.png")
print("Plot written to speed-acc.png")
