# Name : URLProxy.py
# Auth : KT
# Desc : Implimentation of rotating proxy functionality on top of the requests module
import json
import random
import time
from requests import Session

session = Session()

def read_credentials(file_path = 'IO/credentials.json'):
    '''Reads JSON file to load alternative HTML headers, Socks5 proxies, & proxy credentials (user:pass)'''

    with open(file_path, "r") as c:
        data = json.load(c)
        username, password, credentials = data['credentials']
        headers = [header for header in data['headers']]
        proxies = [proxy for proxy in data['proxies']]
    return username, password, credentials, proxies, headers

user, passw, credentials, proxies, headers = read_credentials()

def force_connect(url):
    '''Returns a web request using randomly-chosen Socks5 proxies until 1 works'''

    IP, agent = random.choice(proxies), random.choice(headers) # Random IP, HTML Header
    proxy = "socks5h://" + credentials + '@' + IP + ":1080"    # Combine
    broken_proxies = {}
    url_req = -1

    while url_req == -1:
        try: url_req = session.get(url, proxies={'http':proxy, 'https':proxy}, headers={'User-Agent':agent}, timeout=10) # Use random info
        except Exception: 
            print(f"Connection Error via proxy: {IP}")
            if not broken_proxies.get(IP): 
                broken_proxies[IP] = 0
            # Remove proxy if broken 5 times
            broken_proxies[IP] += 1
            if broken_proxies.get(IP) == 5: 
                proxies.remove(IP)
                if len(proxies) == 0: proxies = list(broken_proxies.keys())
            # Refresh new proxy
            IP, agent = random.choice(proxies), random.choice(headers) # Cycle proxy if connection fails
            proxy = "socks5h://" + credentials + '@' + IP + ":1080"
            time.sleep(1)

    print(f'Successful connection via proxy: {IP}')
    return url_req