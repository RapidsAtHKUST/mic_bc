import os, sys
import numpy as np

if __name__ == "__main__":
    MAX_N = int(sys.argv[1])
    NUM = int(sys.argv[2])
    array = np.random.randint(MAX_N, size=NUM)
    with open("source_vertices_" + str(NUM) + ":" + str(MAX_N) + ".txt", 'wb+') as f:
        for e in array:
            f.write(str(e) + "\n");
