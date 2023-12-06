# Name : URLProxy.py
# Auth : KT
# Desc : Implimentation of rotating proxy functionality on top of the requests module

import json                     # User credential file
import random                   # Random proxy generation
import time                     # Pause between requests
from requests import Session    # Session

session = Session() # Maintain single session to (substantially) increase sequential request speed 

def socks_credentials(file_path = 'IO/credentials.json'):
    '''Reads JSON file to load alternative HTML headers, Socks5 proxies, & proxy credentials (user:pass)'''

    # Open credentials file at specified filepath (default: './IO/credentials.json')
    with open(file_path, "r") as c:
        data = json.load(c)
        username, password, credentials = data['credentials']   # Socks5 proxy credentials
        headers = [header for header in data['headers']]        # Alternative browser headers
        proxies = [proxy for proxy in data['proxies']]          # Alternative proxies
    return username, password, credentials, proxies, headers

user, passw, credentials, IPs, headers = socks_credentials()
Bad_IPs = {}

def force_connect(url, timeout = 5):
    '''Returns a web request using randomly-chosen Socks5 proxies until 1 works'''

    # Instantiate variables
    global IPs, Bad_IPs                                         # Manage IP's outside of function scope
    IP, agent = random.choice(IPs), random.choice(headers)      # Random IP, HTML Header
    proxy = "socks5h://" + credentials + '@' + IP + ":1080"     # Combine

    # Try connection
    r = -1
    while r == -1:
        try: r = session.get(url, proxies={'http':proxy, 'https':proxy}, headers={'User-Agent':agent}, timeout=10) # Request
        except Exception: 
            
            # Connection failure
            print(f"Error: Connection failed via proxy: {IP}")

            # Add broken IP to list
            if not Bad_IPs.get(IP, None): 
                print(f'{IP} -> Bad_IPs dict')
                Bad_IPs[IP] = 0

            # Delete broken proxies that meet timeout threshold
            Bad_IPs[IP] += 1
            if Bad_IPs.get(IP) == timeout: 
                print(f'Deleting {IP} from working IP dict')
                IPs.remove(IP)

            # All proxes have timed out
            if len(IPs) == 0:
                print('All proxies have timed out, clearing Bad_IPs cache')
                IPs = list(Bad_IPs.keys())

            # Refresh new proxy
            IP, agent = random.choice(IPs), random.choice(headers)
            proxy = "socks5h://" + credentials + '@' + IP + ":1080"
            time.sleep(1)

    # Connection success
    print(f'Successful connection via proxy: {IP}')
    return r