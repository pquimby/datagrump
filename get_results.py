import urllib2
import re

f = open('output.txt')
lines = f.readlines()
f.close()

for line in lines:
    (a, b, c, d, url) = line.split(" ")    

    real_url = "http://6829.keithw.org/cgi-bin/6829/stats?data=" + url.split("?")[1] 
    website = urllib2.urlopen(real_url).read()
    regex = r'<strong>([-0-9.]+)</strong>'
    score = re.findall(regex, website)[0]

    regex = r'<b>([-0-9.]+) ms</b>'
    delay = re.findall(regex, website)[0]
    
    regex = r'<b>([-0-9.]+) Mbps</b>'
    tp = re.findall(regex, website)[0]
    
    print a,b,c,d, "\t", float(score), "\t", float(delay), "\t", float(tp)


