import urllib2
import re

f = open('output.txt')
lines = f.readlines()
f.close()

best = (0,0,0,0,-6,"")
for line in lines:
    (a, b, c, d, url) = line.split(" ")    

    real_url = "http://6829.keithw.org/cgi-bin/6829/stats?data=" + url.split("?")[1] 
    website = urllib2.urlopen(real_url).read()
    regex = r'<strong>([-0-9.]+)</strong>'
    score = float(re.findall(regex, website)[0])

    regex = r'<b>([-0-9.]+) ms</b>'
    delay = float(re.findall(regex, website)[0])
    
    regex = r'<b>([-0-9.]+) Mbps</b>'
    tp = float(re.findall(regex, website)[0])
    
    print a,b,c,d, "\t", score, "\t", delay, "\t", tp, "\t", url,
    if score > best[4]:
        best = (a,b,c,d, score, url[:-1])
      

print "Best is:", best


