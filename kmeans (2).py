from pyspark import SparkConf, SparkContext
from pyspark.ml.linalg import Vectors
from pyspark.ml.classification import LogisticRegression
from pyspark.sql import SQLContext
from pyspark.sql.session import SparkSession
from pyspark.ml.feature import VectorAssembler
from pyspark.sql.types import StructType
from pyspark.sql.types import StructField
from pyspark.sql.types import FloatType
from pyspark.sql.types import StringType
from pyspark.mllib.random import RandomRDDs

from pyspark.sql.functions import monotonically_increasing_id
from pyspark.ml.clustering import KMeans
from math import sqrt
import sys
import csv


def loadData(namefile) :

    textfile = sc.textFile(namefile).filter(lambda x: len(x)>0).map(lambda line: line.split(",")).map(lambda x: [float(i) for i in x[:4]] + [x[4].encode("utf-8")]).zipWithIndex().map(lambda x: (x[1], x[0]))
    return textfile

#print rdd1.first()


def loadData2(namefile) :

    textfile = sc.textFile(namefile).filter(lambda x: len(x)>0).map(lambda line: line.split(",")).map(lambda x: [float(i) for i in x[:-1]]).zipWithIndex().map(lambda x: (x[1], x[0]))
    return textfile


# la dimensionalite des donnees 
def dimension(namefile) :
    with open(namefile) as f:
        reader = csv.reader(f)
        row1 = next(reader)
        count = len(row1) 
    return count



# k is the number of centroids #
def initCentroids(k,d):
    centr=[]
    for i in range(0,k) :
        centr.append(RandomRDDs.uniformRDD(sc,d).map(lambda x: 0.0+(10.0-0.0)*x))
    liste =[centr[v].collect()for v in range(0,k)]
    centroids=sc.parallelize(liste).zipWithIndex().map(lambda x: (x[1], x[0]))
    return centroids

#print centroids.first()


# FUNCTION calDistance(...) # 
def calDistance(x,y):
    s = sqrt(sum([(a - b)**2 for a,b in zip(x,y)]))
    dis = float('%.3f'%(s))
    return dis



# FUNCTION closestcentroide(...) #
def closestcentroide(list1):
    cluster = list1[0][0]
    min_dist = list1[0][1]
    for elem in list1:
        if elem[1] < min_dist:
            cluster = elem[0]
            min_dist = elem[1]
    return (cluster,min_dist)


def assignToCluster(rdd,centroids):
    
    union = rdd.cartesian(centroids)
    #((0, [5.1, 3.5, 1.4, 0.2, 'Iris-setosa']), (0, [8.891032299442552, 2.2246616828437027, 6.553598961760145, 3.4613662729800776]))
    # calculer le distance entre les points et chaque centroide
    
    dis = union.map(lambda x: (x[0][0],(x[1][0], calDistance(x[0][1][:-1], x[1][1]))))
    # (0, (0, 7.293456378412841))
   
    distancelist = dis.groupByKey().mapValues(list)
    # (0, [(0, 7.630485299433036), (1, 11.398945930687487), (2, 8.629118276510578)])

    minimumdis = distancelist.mapValues(closestcentroide)
    # (0, (0, 7.630485299433036))
    return minimumdis


# Question 4 : computeCentroids(...) #
def float3chiff(x):
    return float('%.3f'%(x[i]) for i in range(len(x)))

def mean(x,n):
    return [x[i]/n for i in range(len(x))]


def computeCentroids(rdd,findcentroid):

    union = findcentroid.join(rdd)
    # (0, ((1, 5.531), [5.1, 3.5, 1.4, 0.2, 'Iris-setosa']))
    centroid_point = union.map(lambda x: (x[1][0][0],x[1][1][:-1])) 
    # (1, [5.1, 3.5, 1.4, 0.2]), the first element is the number of the cluster
    numpoints = centroid_point.map(lambda x: (x[0],1)).reduceByKey(lambda x,y: x+y)
    # [(0, 98), (1, 52)] means 98 points are attached to cluster 0
    sum = centroid_point.reduceByKey(lambda x,y: [x[i]+y[i] for i in range(len(x))])
    #  [(0, [567.699, 309.600, 336.200, 101.199]), (1, [308.799, 148.5, 227.6, 78.6])]
    union2 = sum.join(numpoints)
    #  [(0, ([567.699, 309.600, 336.200, 101.199], 88)), (1, ([308.799, 148.5, 227.6, 78.6], 62))]

    newcentroids = union2.map(lambda x : (x[0],mean(x[1][0],x[1][1])))
    #   [(0, [5.679, 2.863, 3.473, 1.015]), (1, [6.075, 3.324, 4.162, 1.458])]

    return newcentroids  




# Question 5 : computeIntraClusterDistance(...) #

def computeIntraClusterDistance(rdd,centroids):

    initdistance = assignToCluster(rdd,centroids)

    
    totaldistance = initdistance.map(lambda x: (1,x[1][1])).reduceByKey(lambda x,y: x+y)
   
    return totaldistance



def kmeans(file,k,m):
    

    rdd = loadData2(file)
   
    dim = dimension(file)

    centroids = initCentroids(k,dim-1)
    centroids_copy = centroids

    findcentroid = assignToCluster(rdd,centroids)
    # Nouveau centroides
    newcentroids = computeCentroids(rdd,findcentroid)


   
    #print "\n"
    

    # avant le premier appel computeCentroids()
    total_initcentroide = computeIntraClusterDistance(rdd,centroids)
   
    # apres le premier appel computeCentroids()
    total_newcentroide = computeIntraClusterDistance(rdd,newcentroids)
    # [(1, 291.450)]  
   
       
    convergence = False
    etape = 0

    while not convergence:

        findcentroid = assignToCluster(rdd,centroids)
        # (0, (0, 7.630))
        nouveaucentroides = computeCentroids(rdd,findcentroid)

        findcentroid2 = assignToCluster(rdd,nouveaucentroides)
        total = computeIntraClusterDistance(rdd,nouveaucentroides)
        # compter les points qui changent de cluster
        changecluster = findcentroid.join(findcentroid2).filter(lambda x: x[1][0][0] != x[1][1][0]).count()

        # La condition de convergence est qu'il n'y pas de point qui change de custer. La distance est donc au minimum.
        if changecluster == 0 or etape == m:
            convergence = True
        else:
            centroids = nouveaucentroides
            etape += 1

    finaldistance= computeIntraClusterDistance(rdd,nouveaucentroides)
    finaldis = finaldistance.map(lambda x: x[1])
    
    print "Nombre iteration: ",etape+1
    print "Distance finale: ",finaldis.take(1)
    print "\n"

    return finaldis


###############################################################################

def kmeans2(file,k,m):
    

    rdd = loadData2(file)
   
    dim = dimension(file)

    centroids = initCentroids(k,dim-1)
    centroids_copy = centroids

    findcentroid = assignToCluster(rdd,centroids)
    # Nouveaux centroides
    newcentroids = computeCentroids(rdd,findcentroid)

    convergence = False
    etape = 0

    while not convergence:

        findcentroid = assignToCluster(rdd,centroids)
        # (0, (0, 7.630))
        nouveaucentroides = computeCentroids(rdd,findcentroid)

        findcentroid2 = assignToCluster(rdd,nouveaucentroides)
        total = computeIntraClusterDistance(rdd,nouveaucentroides)
        # compter les points qui changent de cluster
        changecluster = findcentroid.join(findcentroid2).filter(lambda x: x[1][0][0] != x[1][1][0]).count()

        # La condition de convergence est qu'il n'y pas de point qui change de cluster. La distance est donc au minimum.
        if changecluster == 0 or etape == m:
            convergence = True
        else:
            centroids = nouveaucentroides
            etape += 1

    finaldistance= computeIntraClusterDistance(rdd,nouveaucentroides)
    finaldis = finaldistance.map(lambda x: x[1])
    return finaldis


#################################################################################################################
# main.  

if __name__ == "__main__":

    clearconf = SparkConf().setAppName("KMeans")

    sc = SparkContext("local", "KMeans")

    file = sys.argv[1]
    k = int(sys.argv[2])
    m = int(sys.argv[3])

    result = kmeans(file,k,m)
    
    print "Reproduiser l'experience 100 fois: time consuming"
    for i in range(0,100):
        
        t = kmeans2(file,k,m)
        if i == 0:
            union_dis = t
        else:
            union_dis = union_dis.union(t)

    dis_moyenne = union_dis.mean()
    ecart_type = union_dis.stdev()
  
    print "La distance intra cluster moyenne: ",dis_moyenne
    print "l'ecart type: ", ecart_type
           

    

    






