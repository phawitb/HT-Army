import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
import time
import requests
from datetime import datetime

def line_noti(token,msg):
    url = 'https://notify-api.line.me/api/notify'
    headers = {'content-type':'application/x-www-form-urlencoded','Authorization':'Bearer '+token}
    r = requests.post(url, headers=headers, data = {'message':msg})
    return r.text

def create_msg(x,unit):
    w = ''
    t = ''
    if x["flag"] == 'WHITE':
        w = '1/2'
        t = 'ต่อเนื่อง'
    elif x["flag"] == 'GREEN':
        w = '1/2'
        t = '50 นาที พัก: 10 นาที'
    elif x["flag"] == 'YELLOW':
        w = 1
        t = '45 นาที พัก: 15 นาที'
    elif x["flag"] == 'RED':
        w = 1
        t = '30 นาที พัก: 30 นาที'
    elif x["flag"] == 'BLACK':
        w = 1
        t = '20 นาที พัก: 40 นาที'
        
    msg = f'{unit}\n'
    msg += f'⛰สัญญาณธง: {x["flag"]}\n'
    msg += f'⛰อุณหภูมิ: {x["temperature"]} °C\n'
    msg += f'⛰ความชื้นสัมพัทธ์: {x["humidity"]} %\n'
    msg += f'⛰ปริมาณน้ำดื่ม: {w} ลิตร/ชม.\n'
    msg += f'⛰แนะนำให้ฝึก: {t}'

    return msg

def get_time():
    Times = []
    everyhour = False
    for k in db.reference("/UsersData").get().keys():
        ref = db.reference(f"/UsersData/{k}/config/time")
        data = ref.get()
        if data:
            Times += data
            
        ref = db.reference(f"/UsersData/{k}/config/everyhour/status")
        data = ref.get()
        if data:
            everyhour = True
            
        
    Times = [x for x in set(Times) if x]
    
    if everyhour:
        for i in range(0,24):
            if len(str(i)) == 1:
                Times.append(f'0{i}:00')
            else:
                Times.append(f'{i}:00')
    Times = set(Times)
    return Times      

INTERVALTIME_NOTIFY = 300 #sec
cred = credentials.Certificate('ht-army-firebase-adminsdk-yfrij-c875f63e00.json')
firebase_admin = firebase_admin.initialize_app(cred, {'databaseURL': 'https://ht-army-default-rtdb.asia-southeast1.firebasedatabase.app/'})




firsttime = True
while True:
    try:
        if firsttime:
            T = get_time()
            sent = False

        time.sleep(10)

        if sent:
            if datetime.now().strftime("%H:%M") != now:
                now = datetime.now().strftime("%H:%M")
                sent = False
        else:
            now = datetime.now().strftime("%H:%M")

        if datetime.now().strftime("%M") == "29":
            T = get_time()
        

        # now = '08:00'
        if now in T and not sent:
        # while True:
        #     time.sleep(2)
            T = get_time()
            # for k in K:
            for k in db.reference("/UsersData").get().keys():
                #check time match
                ref = db.reference(f"/UsersData/{k}/config/time")
                t = ref.get()
                if t:
                    if now in set(t):
                        print('match...',now)
                        ref = db.reference(f"/UsersData/{k}/last")
                        last = ref.get()
                        
                        #check online
                        print(float(last['timestamp']))
                        if time.time() - float(last['timestamp']) < INTERVALTIME_NOTIFY:
                            print('online')
                            
                            #get token line
                            ref = db.reference(f"/UsersData/{k}/config/line")
                            token = ref.get()
                            #get unit name
                            ref = db.reference(f"/UsersData/{k}/config/unit/name")
                            unit = ref.get()
                            print(unit,token)
                            #create msg
                            msg = create_msg(last,unit)
                            print(msg)
                                
                            #line noti
                            for tok in token:
                                line_noti(tok,msg)
            sent = True

    except:
        print('no internet....')
        time.sleep(10)





