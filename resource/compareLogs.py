

with open("correctTestLog.txt", "r") as f1:
    with open("cpuTrace.txt", "r") as f2:
        index = 0
        while True:
            index += 1
            l1 = f1.readline()
            l2 = f2.readline()

            if (l1 != l2):
                print("Line", index, "not the same")
                break
            elif (l2 == ""):
                print("my trace EOF")
                break
          
