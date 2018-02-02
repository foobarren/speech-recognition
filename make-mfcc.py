# encoding: UTF-8

import os
import sys
import json 
import urllib

wavdir =os.path.join(sys.path[0],"waves")
basemfccdir =os.path.join(sys.path[0],"basemfcc")
mfccdir =os.path.join(sys.path[0],"mfcc")

def makejson():
    mfcclist =[]
    for ( root, dirs, files ) in os.walk(wavdir):
        for filename in files:
            filepatterns = filename.split('.')
            if filepatterns[1] =='wav':
                mfcclist.append({'file':filename,'keytime':'2000'})

    with open(os.path.join(sys.path[0],"wavelist.json"),"w") as f:
        json.dump(mfcclist,f, indent=4,ensure_ascii=False) 

def convertToUX(content):
    tmp = ""
    for cr in content:
            if (cr=="u" or cr=="'"):
                    tmp = tmp + cr
                    continue
            tmp = tmp + repr(cr).replace("u'", "").replace("'","")
    return tmp.decode("unicode_escape").encode("utf-8")

def renameall():
    mfcclist =[]
    os.chdir("basemfcc")
    for ( root, dirs, files ) in os.walk(basemfccdir):        
        for filename in files:
            filepatterns = filename.split('.')
            if filepatterns[2] =='mfcc':                 
                newwave_filename =  filename.replace('new_','')   
                command = r'mv ' +filename + r' ' + newwave_filename 
                try:
                    os.system(command) 
                except:
                    print wave_filename          
    os.chdir("..")

def makemfcc():
    mfcclist =[]
    with open("wavelist.json") as f:
        mfcclist = json.load(f,encoding="UTF-8")

    os.chdir("basemfcc")
    for info in mfcclist:
        beginline= int(info['keytime'])/48+1
        filename = info['file'].encode('utf-8') + ".mfcc"
        # print filename
        print filename
        # filename = info['file'].unicode("utf8")
        with open(filename) as f:
            linelist = f.readlines()[beginline:beginline+20]
        with open(r'../mfcc/'+filename,'w') as f:
            f.write('20\n')
            f.writelines(linelist)
      
    os.chdir("..")
    
# renameall()
makemfcc()
