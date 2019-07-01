
#-*- coding: utf-8 -*-


"""Detects text in the file."""
import time
import picamera
from google.cloud import vision
from googletrans import Translator
import pymysql
import os

eng = ''
kor = ''
def func():
    global eng
    global kor
    try:
        with picamera.PiCamera() as camera:
            camera.start_preview(fullscreen=True) #, window=(100,20,640,480))
            time.sleep(7)
            camera.capture('image.jpg')
            os.system("xdg-open image.jpg")
            camera.stop_preview()
        
        client = vision.ImageAnnotatorClient()

        with open('./image.jpg', 'rb') as image_file:
            content = image_file.read()

        image = vision.types.Image(content=content)

        response = client.text_detection(image=image)
        texts = response.text_annotations
        print('Texts:')

        eng = texts[0].description
        translator = Translator()
        kor = translator.translate(eng, src='en', dest='ko').text
        print("======")
        print(eng)
        print("======")
        print(kor)
        print("======")
    except Exception as e:
        eng = " "
        kor = " "
        print(e)


read_sql = '''
        select press from project_data order by idx desc limit 1
      '''

write_sql = '''
                insert into project_data (eng, kor) values (%s, %s)
            '''


conn = pymysql.connect(host='IP', user='root', password='password',db='project', charset='utf8')
while True:
    
    curs = conn.cursor()
    curs.execute(read_sql)
    row = curs.fetchone()[0]
    print(row)
    if row == 1:
        func()
        curs.execute(write_sql,(eng, kor))
        
        print(eng, kor)
        
    conn.commit()

conn.close()
    



