import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
import joblib
from sklearn.tree import export_text

# 加载数据
def load_data(num_samples=None):
    print("loading data...")
    X = []  # 输入特征
    y = []  # 输出标签
    file_list = ["dhry_log"]
    for filename in file_list:
        with open(filename, "r") as file:
            lines = file.readlines()
            total_samples = len(lines) // 2  # 数据是成对出现的，每两行为一个样本
            # 如果 num_samples 未指定或超出总数，则加载所有数据
            if num_samples is None or num_samples > total_samples:
                num_samples = total_samples
        for i in range(0, 2 * num_samples, 2):  # 每次处理两行
            # 处理第一行（特征）
            features = [int(char) for char in lines[i].strip()]
            X.append(features)
            # 处理第二行（标签）
            label = int(lines[i + 1].strip(),2)
            y.append(label)
    # 打印加载的数据形状
    print(f"Loaded {len(X)} samples")
    print(f"X shape: {np.array(X).shape}")
    print(f"y shape: {np.array(y).shape}")
    #统计不同标签的数量
    unique_labels = np.unique(y)
    print(f"Label counts: {len(unique_labels)}")
    #打印每个标签的数量
    for label in unique_labels:
        print(f"Label {label} count: {y.count(label)}")
    return np.array(X), np.array(y)



# 示例用法
if __name__ == "__main__":
    model_filename = 'random_forest_model_5.joblib'
    loaded_model = joblib.load(model_filename)
    print(f"Model loaded from {model_filename}")
    X, y = load_data()
    model_score = loaded_model.score(X, y)
    print(f"Model score: {model_score}")
