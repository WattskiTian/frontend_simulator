import os

def count_code_lines(directory):
    total_lines = 0
    file_types = ('.cpp', '.h', 'Makefile')

    for root, dirs, files in os.walk(directory):
        for filename in files:
            filepath = os.path.join(root, filename)
            
            # check file type
            if filename.endswith(('.cpp', '.h')) or filename == 'Makefile':
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        lines = sum(1 for line in f)
                        total_lines += lines
                        print(f"{filepath}: {lines} lines")
                except Exception as e:
                    print(f"cannot read file {filepath}: {str(e)}")
    
    return total_lines

if __name__ == "__main__":
    current_dir = os.getcwd()
    print("counting code lines...\n")
    total = count_code_lines(current_dir)
    print(f"\nTotal code lines: {total}")