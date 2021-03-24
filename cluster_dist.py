import numpy as np
import pandas as pd
import cv2 as cv
from matplotlib import pyplot as plt

# v1 completed : 1640hrs 16th Mar, 2021
# the algorithm here is not optimized

def viewImage(image):
    cv.namedWindow("image", cv.WINDOW_NORMAL)
    cv.imshow("image", image)
    cv.waitKey()

path = "C:\\Users\\Anil Myne\\Desktop\\flower-background.jpg"
path = "C:\\Users\\Anil Myne\\Desktop\\AIsets\\leaf.PNG"


img = cv.imread(path)
imgg = cv.imread(path, 0)
imgh = cv.cvtColor(img, cv.COLOR_BGR2HSV)

imgg[imgg == 244] = 243
imgg[imgg == 247] = 248

palette = np.unique(imgg)

h, w = imgg.shape
fpath = "C:\\Users\\Anil Myne\\Desktop\\codes\\col\\features.csv"
# path from which to get features : made with C++

print("Loading...")
df = pd.read_csv(fpath, header=None, index_col=None)
df.drop(df.columns[-1], axis=1, inplace=True)

features = df.iloc[:, :]
y = df.iloc[:, -1]
print("Loaded...")

n_cl = 2

X = features

def uniques(X, colorcol=None, indx=-1):
    clrs = []
    if colorcol is None:
        colorcol = X.iloc[:, indx]
        X = X.iloc[:, :-1]
        clrs = np.unique(colorcol)
    else:
        clrs = np.unique(np.array(colorcol))
    sortedmeans = []
    #print('colors:', clrs)
    try:
        print("Image Compatibility:", (clrs == palette).all())
    except:
        try:
            print("Image Compatibility:", (clrs == palette))
        except:
            pass
    for clr in clrs:
        cs = X[colorcol == clr]
        #x, y = np.where(cs == cs.min()) # cases of many mins
        # scenario: count average dist away for each color <----
        # scenario: count min/next
        cmeans = np.mean(cs, axis=0) # average distance per color
        asort = np.argsort(cmeans)
        cmeans = (clrs[asort], cmeans[asort])
        sortedmeans.append(cmeans)
    return np.array(sortedmeans)

def getweight():
    pass

def cluster(sortedmeans, cascade=False, thresh=.5, k=0):
    # cluster engine, cluster by reverse cmeans
    # Take off first key in each & check for coincidence, merge if req
    x, y, z = sortedmeans.shape # X, 2, csize
    # [0, 127, 142, 150, 203, 233, 255]
    cycle = 0
    clusters, cdists = [[] for i in range(x)], [[] for i in range(x)]
    finished, f_index = [], []
    scolors, means = sortedmeans[:, 0], sortedmeans[:, 1]
    for i in range(x):
        clr = scolors[i, cycle]
        mn = means[i, cycle]
        if clr not in finished:
            finished.append(clr)
            f_index.append(i)
            clusters[i].append(clr)
            cdists[i].append(mn)
        else:
            idx = finished.index(clr)
            idx_ = f_index[idx]
            clusters[idx_].append(clr)
            cdists[idx_].append(mn)
    return clusters

# c[:, 0]
# 255 votes to be member of 0, & not 255 itself
# 0:0->1, 0:255->68.9, 255:0->1.0, 255:255->653; use thresh 1.0/68.9
# non matching colors get added to the current, but wont use mean or with weighted mean
# 1 - ( x / sum(means))

def wmean(a, b):
    if a:
        wsum = 0
        for i, v in enumerate(a):
            wsum += (b[i] * v)
        return wsum / len(a)
    return 0.0

print("draw")
    
def kcluster(sortedmeans, k=0, thresh=.5, cascade=False, palette=None, prev_cluster_indices=None):
    """ cluster colors by column k
    @params"""
    x, y, z = sortedmeans.shape # csize, 2, csize
    scolors, means = sortedmeans[:, 0], sortedmeans[:, 1]
    if palette is None:
        palette = np.unique(scolors)
    indices = {clr:i for i, clr in enumerate(palette)}
    clusters, wgts = [[clr] for clr in palette], [[0.] for i in range(x)]
    finished, f_index = [], []

    for i in range(x):
        clk = scolors[i][k]
        mnk = means[i][k]
        indxk = indices[clk]
        smeans = []
        order = 0
        if len(clusters[i]) == 0:
            continue
        for cls in clusters[i]:
            indxs = np.where(scolors[indxk] == cls)[0][0] # k:c
            mns = means[indxk][indxs] # get mean there
            if mns > mnk:
                order += 1
            smeans.append(min(mns, mnk) / max(mnk, mns))
        wgt = np.mean(smeans)
        order = int(order/len(smeans))
        swap = True
        if cascade:
            if wgt < thresh:
                swap = False  # do nothing
            else: # >= thresh
                idx  = clusters[indxk].index(clk)
                if wgt > wgts[indxk][idx]: # wgt > currwgt, != 1.?
                    pass
                else:
                    swap = False
        # TRY: (1 - wgt) on reverse-order
        if swap:
            if order == 1: # move self.cluster to other.cluster
                for j, c in enumerate(clusters[i]):
                    indices[c] = indxk
                    if c not in clusters[indxk]:
                        clusters[indxk].append(c)
                        wgts[indxk].append(smeans[j])
                    else:
                        idx = clusters[indxk].index(c)
                        wgts[indxk][idx] = smeans[j]
                clusters[i] = []
                wgts[i] = []
            else: # move other.const to self.cluster
                idx  = clusters[indxk].index(clk)
                clusters[indxk].remove(clk)
                wghtk = wgts[indxk].pop(idx)
                indices[clk] = i
                if clk not in clusters[i]:
                    clusters[i] += [clk]
                    wgts[i] += [wgt]
                else:
                    idx  = clusters[i].index(clk)
                    wgts[i][idx] = wgt
    return clusters, wgts

def maxclr(aa, bb):
    if bb:
        idx = bb.index(max(bb))
        return aa[idx]
    return []

a, b = kcluster(uniques(X))
tr = map(maxclr, a, b)
db = [v for v in tr]
dd = dict()

for i in range(len(a)):
    for itm in a[i]:
        dd[int(itm)] = int(db[i])

u, inv = np.unique(imgg,return_inverse=True)
imm = np.array([dd[x] for x in u])[inv].reshape(imgg.shape)
imm = imm.astype(np.uint8)

#viewImage(imgg) # original
#viewImage(imm)  # compressed





























