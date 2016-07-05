#!/apps/python/PRO/bin/python
from subprocess import call
import sys,os,time

def main():
    
    _email="ciprian@jlab.org"
    _source="/lustre/expphy/work/hallc/qweak/ciprian/simCodeG410/msc"
    _directory="/lustre/expphy/volatile/hallc/qweak/ciprian/farmoutput/msc/mott/1e2"
    _nEv=300000
    _nrStop=200
    _nrStart=0
    _pol="V"
    _units=0
    submit=1

    idRoot= _pol+'_simple_%03dk'% (_nEv/1000) 
    for nr in range(_nrStart,_nrStop): # repeat for nr jobs
        _idN= idRoot+'_%04d'% (nr) 
        print _idN
        createMacFile(_directory,_idN,_nEv,nr,_pol)
        call(["cp",_source+"/build/msc",_directory+"/"+_idN+"/msc"])

    createXMLfile(_source,_directory,idRoot,_nrStart,_nrStop,_email)

    if submit==1:
        print "submitting simple sim ",_idN," between ",_nrStart,_nrStop
        call(["jsub","-xml",_source+"/scripts/jobs/"+idRoot+".xml"])
    else:
        print "NOT submitting position sampled with id",_idN," between ",_nrStart,_nrStop
        
    print "I am all done"

def createMacFile(directory,idname,nEv,nr,_pol):
    if not os.path.exists(directory+"/"+idname):
        os.makedirs(directory+"/"+idname)
   
    f=open(directory+"/"+idname+"/myRun.mac",'w')
    seedA=int(time.time()/2000.)+   100*nr+nr
    seedB=int(time.time()/300. ) +10000*nr+nr
    f.write("/random/setSeeds "+str(seedA)+" "+str(seedB)+"\n")
    f.write("/msc/det/setStepSizeG4 -0.01 mm\n")
    f.write("/msc/setNrUnits 0\n")
    f.write("/msc/PrimaryEventGen/beamEnergy 1160 MeV\n")
    f.write("/msc/PrimaryEventGen/setPolarization "+_pol+"\n")
    f.write("/msc/SteppingAction/setWriteTree 0\n")
    f.write("/msc/SteppingAction/setWriteANdata 0\n")
    f.write("/msc/SteppingAction/initOutput 1\n")
    f.write("/msc/event/setPrintModulo 50000\n")
    f.write("/run/beamOn "+str(nEv)+"\n")
    f.close()
    return 0

def createXMLfile(source,writeDir,idRoot,nStart,nStop,email):
    
    if not os.path.exists(source+"/scripts/jobs"):
        os.makedirs(source+"/scripts/jobs")

    f=open(source+"/scripts/jobs/"+idRoot+".xml","w")
    f.write("<Request>\n")
    f.write("  <Email email=\""+email+"\" request=\"false\" job=\"true\"/>\n")
    f.write("  <Project name=\"qweak\"/>\n")
#    f.write("  <Track name=\"debug\"/>\n")
    f.write("  <Track name=\"simulation\"/>\n")
    f.write("  <Name name=\""+idRoot+"\"/>\n")
    f.write("  <OS name=\"centos65\"/>\n")
    f.write("  <Command><![CDATA[\n")
    f.write("msc -m myRun.mac\n")
    f.write("  ]]></Command>\n")
    f.write("  <Memory space=\"2000\" unit=\"MB\"/>\n")

    for nr in range(nStart,nStop): # repeat for nr jobs
        f.write("  <Job>\n")
        idName= writeDir+"/"+idRoot+'_%04d'%(nr)
        f.write("    <Input src=\""+idName+"/msc\" dest=\"msc\"/>\n")
        f.write("    <Input src=\""+idName+"/myRun.mac\" dest=\"myRun.mac\"/>\n")
        f.write("    <Output src=\"o_msc.root\" dest=\""+idName+"/o_msc.root\"/>\n")
        f.write("    <Stdout dest=\""+idName+"/log/log.out\"/>\n")
        f.write("    <Stderr dest=\""+idName+"/log/log.err\"/>\n")
        f.write("  </Job>\n\n")

    f.write("</Request>\n")
    f.close()
    return 0
            
if __name__ == '__main__':
    main()
                            
