import matter
import string

print("MTR: Auto-config script loaded")

def auto_configure_matter()
    print("MTR: Checking for Matter Device...")
    var drivers = []
    try
        if tasmota._drivers
            drivers = tasmota._drivers
        end
    except .. as e, m
        print(format("MTR: Error accessing tasmota._drivers: %s - %s", e, m))
    end

    var dev = nil
    
    if drivers
        for d: drivers
            try
                if isinstance(d, matter.Device) || str(d).find("Matter_Device") >= 0
                    dev = d
                    print("MTR: Found Matter Device instance")
                    break
                end
            except ..
            end
        end
    else
        print("MTR: No drivers found in tasmota._drivers")
    end

    if dev == nil
        print("MTR: Matter Device not found. Retrying in 5s...")
        tasmota.set_timer(5000, auto_configure_matter)
        return
    end

    print("MTR: Found Matter Device, checking endpoints...")

    # Ensure plugins_config is initialized
    if dev.plugins_config == nil
        print("MTR: plugins_config is nil, initializing...")
        dev.plugins_config = {}
    end

    # --- Configure Presence (Occupancy) Endpoint ---
    # Note: Using "Presence" (correct spelling). 
    # If "Pressence" (old typo) exists, user should remove it manually or reset.
    var has_presence = false
    for k: dev.plugins_config.keys()
        var conf = dev.plugins_config[k]
        if conf.find("type") == "v_occupancy" && (conf.find("name") == "Presence" || conf.find("name") == "Pressence")
            has_presence = true
            print("MTR: Presence endpoint already exists.")
            break
        end
    end

    if !has_presence
        print("MTR: Adding Presence endpoint (v_occupancy)...")
        try
            dev.bridge_add_endpoint("v_occupancy", {"name": "Presence"})
            print("MTR: Presence endpoint added successfully.")
        except .. as e, m
            print(format("MTR: Error adding Presence endpoint: %s - %s", e, m))
        end
    end

    # --- Configure Illuminance Endpoint ---
    var has_illuminance = false
    for k: dev.plugins_config.keys()
        var conf = dev.plugins_config[k]
        if conf.find("type") == "illuminance" && conf.find("name") == "Illuminance" && conf.find("filter") == "Illuminance"
            has_illuminance = true
            print("MTR: Illuminance endpoint already exists.")
            break
        end
    end

    if !has_illuminance
        print("MTR: Adding Illuminance endpoint (illuminance)...")
        try
            dev.bridge_add_endpoint("illuminance", {"name": "Illuminance", "filter": "Illuminance"})
            print("MTR: Illuminance endpoint added successfully.")
        except .. as e, m
            print(format("MTR: Error adding Illuminance endpoint: %s - %s", e, m))
        end
    end
    
    print("MTR: Auto-configuration check complete.")
end

# Start the configuration check after 10 seconds to allow Matter to fully initialize
tasmota.set_timer(10000, auto_configure_matter)

# -------------------------------------------------------------------------
# Auto-Sync Topic with DeviceName
# -------------------------------------------------------------------------
def sync_topic()
    print("AUTO-SYNC: Starting check...")
    
    # Get DeviceName
    var res_dn = tasmota.cmd("DeviceName")
    if res_dn == nil
        print("AUTO-SYNC: Error - DeviceName response is nil")
        return
    end
    if !res_dn.contains("DeviceName")
        print("AUTO-SYNC: Error - Response does not contain DeviceName")
        return
    end
    var dev_name = res_dn["DeviceName"]
    print(string.format("AUTO-SYNC: DeviceName is '%s'", dev_name))
    
    # Get Current Topic
    var res_top = tasmota.cmd("Topic")
    if res_top == nil
        print("AUTO-SYNC: Error - Topic response is nil")
        return
    end
    if !res_top.contains("Topic")
        print("AUTO-SYNC: Error - Response does not contain Topic")
        return
    end
    var cur_topic = res_top["Topic"]
    print(string.format("AUTO-SYNC: Current Topic is '%s'", cur_topic))
    
    # Sync if different
    if dev_name != cur_topic
        print(string.format("AUTO-SYNC: Updating Topic (%s) -> (%s)", cur_topic, dev_name))
        tasmota.cmd("Topic " + dev_name)
    else
        print("AUTO-SYNC: Topic is already synchronized.")
    end
end

# Trigger sync when Wifi connects (with delay to ensure system is ready)
tasmota.add_rule("Wifi#Connected", def() tasmota.set_timer(2000, sync_topic) end)

# Check if already connected at startup
if tasmota.wifi().find("ip")
    print("AUTO-SYNC: Wifi already connected, scheduling sync...")
    tasmota.set_timer(2000, sync_topic)
end
