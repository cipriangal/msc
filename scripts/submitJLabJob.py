#!/apps/python/PRO/bin/python
from subprocess import call
import sys
import os
import time

def main():
    
    _email="ciprian@jlab.org"
    _source="/w/hallc-scifs2/qweak/ciprian/simCodeG410/msc"
    _directory="/lustre/expphy/volatile/hallc/qweak/ciprian/farmoutput/msc/mott/1e2"
    _nEv=500000
    _nrStop=100
    _nrStart=0
    _pol="V"
    _units=15
    submit=1
    
    for nr in range(_nrStart,_nrStop): # repeat for nr jobs
        _idN= _pol+'_%03d_%03d'% (_units,nr) 
	createMacFile(_directory,_idN,_nEv,nr)
	createXMLfile(_idN,_directory,_email,_source)
        call(["cp",_source+"/build/msc",_directory+"/jobs/"+_idN+"/msc"])
        
	if submit==1:
            print "submitting for the ",nr,"th time"
            call(["jsub","-xml",_directory+"/"+_idN+"/job.xml"])
	else:
            print "do not submit "
            
    print "I am all done"

def createMacFile(directory,idname,nEv,nr):
    if not os.path.exists(directory+"/"+idname):
        os.makedirs(directory+"/"+idname)
   
    f=open(directory+"/"+idname+"/myRun.mac",'w')
    seedA=int(time.time())+      1000000*nr+nr
    seedB=int(time.time()*100)+100000000*nr+nr
    f.write("/random/setSeeds "+str(seedA)+" "+str(seedB)+"\n")
    f.write("/msc/event/setPrintModulo 100000\n")
    f.write("/run/beamOn "+str(nEv)+"\n")
    f.close()
    return 0

def createXMLfile(idname,directory,email,source):
    if not os.path.exists(directory+"/"+idname+"/log"):
        os.makedirs(directory+"/"+idname+"/log")
    
    f=open(directory+"/"+idname+"/job.xml","w")
    f.write("<Request>\n")
    f.write("  <Email email=\""+email+"\" request=\"false\" job=\"true\"/>\n")
    f.write("  <Project name=\"qweak\"/>\n")
    f.write("  <Track name=\"simulation\"/>\n")
    f.write("  <Name name=\""+idname+"\"/>\n")
    f.write("  <OS name=\"centos65\"/>\n")
    f.write("  <Command><![CDATA[\n")
    f.write("cd "+directory+"/jobs/"+idname+"\n")
    f.write("msc -m myRun.mac\n")
    f.write("  ]]></Command>\n")
    f.write("  <Memory space=\"1200\" unit=\"MB\"/>\n")
    f.write("  <TimeLimit unit=\"minutes\" time=\"4320\"/>\n")
    f.write("  <Job>\n")
    f.write("    <Stdout dest=\""+directory+"/"+idname+"/log/log.out\"/>\n")
    f.write("    <Stderr dest=\""+directory+"/"+idname+"/log/log.err\"/>\n")
    f.write("  </Job>\n")
    f.write("</Request>\n")
    f.close()

    return 0

            
if __name__ == '__main__':
    main()
                            
