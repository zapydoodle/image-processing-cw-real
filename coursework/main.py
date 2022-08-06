import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import imageio

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
    img = imageio.imread("dart14,jpg")
    thetas = np.deg2rad(np.arange(-90.0, 90.0, angle_step))
    rhos = np.linspace(-diag_len, diag_len, diag_len * 2)
    plt.imshow(houghSpace, aspect='auto', cmap='jet',
               extent=[np.rad2deg(thetas[-1]), np.rad2deg(thetas[0]), rhos[-1], rhos[0]])

    plt.show()







# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
