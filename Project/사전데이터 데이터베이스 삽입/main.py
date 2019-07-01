import pymysql

import io

f = io.open('engkor.txt','r', encoding='cp949')
count = 1

conn = pymysql.connect(host='IP', user='root', password='password',db='project', charset='utf8')
while True:
    try:
        line = f.readline()
        if not line: break
        
        line = line.replace('\n','')
        idx = line.find("///")
        eng = line[:idx]
        kor = line[idx+4:]

        write_sql = '''
                        insert into dictionary (eng, kor) values (%s, %s)
                    '''
        curs = conn.cursor()
        curs.execute(write_sql, (eng, kor))
        conn.commit()
        print(count)
        count += 1
        
    except Exception as e:
        print(count)
        count += 1
        continue
    

conn.close()
