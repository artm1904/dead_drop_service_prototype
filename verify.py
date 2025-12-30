import requests
import time
import subprocess
import sys

def verify():
    # Start the server
    server = subprocess.Popen(["./build/dead_drop_service"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print("Server started...")
    time.sleep(2) # Give it time to start

    try:
        base_url = "http://localhost:8080"
        
        # 1. Test Key Creation
        print("Test 1: Creating secret...")
        secret_text = "This is a super secret message"
        resp = requests.post(f"{base_url}/api/secret", json={"secret": secret_text})
        if resp.status_code != 200:
            print(f"FAILED: Create secret returned {resp.status_code}")
            return False
        
        data = resp.json()
        if "id" not in data:
            print("FAILED: No ID returned")
            return False
        
        secret_id = data["id"]
        print(f"Success. Secret ID: {secret_id}")

        # 2. Test Key Retrieval (First time) - SHOULD SUCCEED
        print("Test 2: Retrieving secret (1st time)...")
        resp = requests.get(f"{base_url}/secret/{secret_id}")
        if resp.status_code != 200:
            print(f"FAILED: Get secret returned {resp.status_code}")
            return False
        
        if secret_text not in resp.text:
            print("FAILED: Secret text not found in response")
            print("Response:", resp.text)
            return False
        print("Success. Secret retrieved.")

        # 3. Test Key Retrieval (Second time) - SHOULD FAIL
        print("Test 3: Retrieving secret (2nd time)...")
        resp = requests.get(f"{base_url}/secret/{secret_id}")
        if resp.status_code != 404:
            print(f"FAILED: Get secret 2nd time returned {resp.status_code}, expected 404")
            return False
        print("Success. Secret already deleted (404).")

        print("ALL TESTS PASSED")
        return True

    except Exception as e:
        print(f"ERROR: {e}")
        return False
    finally:
        server.terminate()
        server.wait()

if __name__ == "__main__":
    if verify():
        sys.exit(0)
    else:
        sys.exit(1)
