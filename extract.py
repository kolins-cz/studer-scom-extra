import yaml

# Open the YAML file and load its content
with open('ha.yaml', 'r') as yaml_file:
    data = yaml.safe_load(yaml_file)

# Open the text file to write the names
with open('names.txt', 'w') as txt_file:
    for key, item in data.items():
        # Extract the name and write it to the text file
        name = item.get('name')
        if name:
            txt_file.write(name + '\n')