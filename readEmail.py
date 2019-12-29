
"""

readEmail.py: 

This program checks a gmail account for the latest report from Whisper Earnings.  
When a new report is found the message body is parsed and stored in a text file (emailText.txt)

__author__      = "Tony Egizii
__copyright__   = "Copy Right 2019 - 3019"

"""

#********System Imports************

from googleapiclient.discovery import build
from httplib2 import Http
from oauth2client import file, client, tools
import os
from bs4 import BeautifulSoup as bs4
import base64
import datetime

#***********************************


SCOPES = 'https://www.googleapis.com/auth/gmail.readonly'

#--main--#
####################

#--This function looks for the email account token.  
#--If not found it will create a new one using the account credentials.

#with open('dateOfLastReport.txt', 'w+') as f:
#    f.write('1 Jan 2000')
    
####################

def main():
   
    store = file.Storage('token.json')
    creds = store.get()
    if not creds or creds.invalid:
        flow = client.flow_from_clientsecrets('credentials.json', SCOPES)
        creds = tools.run_flow(flow, store)
    service = build('gmail', 'v1', http=creds.authorize(Http()))
    return service


#--emailFind--#
####################

#--Get a list of email ID's from the inbox.
#--Look at the message object and check the "FROM" value for the sender we are looking for.
#--Return the ID of that message

####################

def emailFind(service):
    # Call the Gmail API to fetch INBOX
    results = service.users().messages().list(userId='me',labelIds = ['INBOX']).execute()
        
    #--The get() used below is the python get() not the gmail api get() !@#$%^&*(*&^%$

    try:
        messages = results.get('messages', [])

        print ("Gmail login successful")
    except None:
        print ("No new message found")
      
    for message in messages:
        msg = service.users().messages().get(userId='me', id=message['id']).execute()
        
        
        for i in msg['payload']['headers']:
            if i['name'] == 'From' and '<epsguidance@earningswhispers.com>' in i['value']:
                reportId = (msg['id'])
                #print ('Message Snippet: \n', msg['snippet'],'\n')
                return reportId


#--getDate--#
####################

####################

def getDate(id):
    msg = service.users().messages().get(userId='me', id=id).execute()
    
    for i in msg['payload']['headers']:
         if i['name'] == 'Date':
            return (i['value'])
    return

    
#--emailRawRead--#
####################

#--The function below takes the message ID from the previous function, 
#--pulls the message data and converts it to html

####################

def emailRawRead(id):
    store = file.Storage('token.json')
    creds = store.get()
    if not creds or creds.invalid:
        flow = client.flow_from_clientsecrets('credentials.json', SCOPES)
        creds = tools.run_flow(flow, store)
    service = build('gmail', 'v1', http=creds.authorize(Http()))
    message = service.users().messages().get(userId='me', id=id).execute()
    pl = message['payload']['body']['data']
    return pl


#--toText--#
####################


####################

def toText(parsed):


    clean_one = parsed.replace("-","+") # decoding from Base64 to UTF-8
    clean_one = clean_one.replace("_","/") # decoding from Base64 to UTF-8
    clean_two = base64.b64decode (bytes(clean_one, 'UTF-8')) # decoding from Base64 to UTF-8
    soup = bs4(clean_two , "lxml" )
    mssg_body = soup.body()
    return mssg_body


#--dateSave--##
####################

####################

def dateSave(date):
    cDate = datetime.datetime.strptime(date, '%d %b %Y')
    f = open(r'dateOfLastReport.txt', 'r')

    lastDate = f.read()
    lDate = datetime.datetime.strptime(lastDate[:11], '%d %b %Y')
    f.close()

    if cDate > lDate:
        f = open(r'dateOfLastReport.txt', 'w+')
        x = cDate.strftime('%d %b %Y')
        print ("New report issued on: \n" + date)
        f.write(x)
        f.close()
        return True
        
    return False


#--toFile--##
####################

####################

def toFile(parsed):
    file = open(r"emailText.txt", "w+")
    for i in parsed:
        file.write(i.text + '\n')
    file.close()



if __name__ == '__main__':
    #--service holds the credentials needed to access the gmail account
    service = main()
    #--reportId holds the ID of the report email
    reportId = emailFind(service) 
    reportDate = getDate(reportId)[:-15]
    if dateSave(reportDate)==True:
        raw = emailRawRead(reportId)
        i = toText(raw)
        toFile(i)
        print('New report saved')
    else: print ('No New Report')    
   
