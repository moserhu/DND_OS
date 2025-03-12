import requests
import json

# API URL (Replace with your actual API IP)
API_URL = "http://192.168.1.222:8000/profiles"  # Replace X with correct IP
LOCAL_JSON_FILE = "/home/dnd1/Documents/DND_Screen/health_data.json"  # Update path

def fetch_and_save_profiles():
    try:
        response = requests.get(API_URL)
        if response.status_code == 200:
            profiles = response.json()

            # Save to health_data.json (Overwrite existing data)
            with open(LOCAL_JSON_FILE, "w") as file:
                json.dump(profiles, file, indent=4)

            print("✅ Data saved successfully to health_data.json.")

        else:
            print(f"❌ Failed to fetch data. HTTP Status: {response.status_code}")

    except Exception as e:
        print(f"⚠️ Error fetching profiles: {e}")

# Run the function at startup
if __name__ == "__main__":
    fetch_and_save_profiles()
