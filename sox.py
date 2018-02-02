
# encoding: UTF-8
import os

dir =r"/data/audio/speech-recognition/compare"

for ( root, dirs, files ) in os.walk(dir):
    for filename in files:
        filepatterns = filename.split('.')
        if filepatterns[1] =='wav':
            wave_filename =  os.path.join(root,filename)  
            newwave_filename =  os.path.join(root,'new_'+filename)   
            command = r'sox -t wav -c 1 -A -b 8 -r 8000 ' +wave_filename + r' -t wav -c 1 -e signed-integer -b 16 -r 8000  ' + newwave_filename           
            try:
                os.system(command) 
            except:
                print wave_filename