import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib.lines as mlines

def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.




def main():
    houghSpace=[]
    with open("hough_space.txt") as houghFile:
        for i, line in enumerate(houghFile.read().split("\n")):
            row=[]
            if(len(line)!=0):
                for j, val in enumerate(line.split(",")):
                    if(val!=""):
                        row.append(int(val))
                print(len(row))

                houghSpace.append(row)









    houghSpace.pop(-1)
    #print(houghSpace)
    houghSpaceArr=np.array(houghSpace)
    print(houghSpaceArr)
    plt.plot(houghSpaceArr)
    plt.show()






# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
