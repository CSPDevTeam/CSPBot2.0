import re
lastLog = "12:49:12 INFO [Server] There are 0/10 players online:"
matchObj = re.search(r"are\s(.+)\/(.+)\splayers",lastLog)
print(matchObj.group(1),"/",matchObj.group(2))