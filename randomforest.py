import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split

# 1. 数据读取函数
def load_data(filename):
    print("loading data...")
    X = []  # 输入特征列表
    y = []  # 输出标签列表
    
    with open(filename, 'r') as file:
        lines = file.readlines()
        for i in range(0, len(lines)-1, 2):  # 每次处理两行
            # 处理第一行（特征）
            features = [int(x) for x in lines[i].strip().split()]
            X.append(features)
            # 处理第二行（标签）
            label = int(lines[i + 1].strip())
            y.append(label)
    # print shape of X and y
    print(np.array(X).shape)
    print(np.array(y).shape)
    return np.array(X), np.array(y)

# 2. 主程序
def train_random_forest(filename):
    # 加载数据
    X, y = load_data(filename)
    
    # 将数据分为训练集和测试集
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, 
        test_size=0.2,    # 20%的数据用于试
        random_state=42   # 固定随机种子以保证可重复性
    )
    
    # 创建并配置随机森林分类器
    rf_classifier = RandomForestClassifier(
        n_estimators=100,         # 决策树数量
        max_depth=10,            # 最大树深
        min_samples_split=2,     # 节点分裂所需的最小样本数
        min_samples_leaf=1,      # 叶子节点最小样本数
        criterion='gini',        # 分裂质量的衡量标准
        random_state=42          # 随机种子
    )
    
    # 训练模型
    rf_classifier.fit(X_train, y_train)
    
    # 测试模型
    train_score = rf_classifier.score(X_train, y_train)
    test_score = rf_classifier.score(X_test, y_test)
    
    print(f"训练集准确率: {train_score:.4f}")
    print(f"测试集准确率: {test_score:.4f}")
    
    # 从测试集中随机选择5个样本并显示预测结果
    print("\n测试集前5个样本的预测结果：")
    # 随机选择5个索引
    num_samples = min(5, len(X_test))  # 确保不超过测试集大小
    random_indices = np.random.choice(len(X_test), num_samples, replace=False)
    
    # 获取预测结果
    predictions = rf_classifier.predict(X_test[random_indices])
    
    # 显示结果
    for i, idx in enumerate(random_indices):
        print(f"\n样本 {i + 1}:")
        print(f"输入特征: {X_test[idx]}")
        print(f"真实标签: {y_test[idx]}")
        print(f"预测标签: {predictions[i]}")
        print(f"预测是否正确: {predictions[i] == y_test[idx]}")
    
    return rf_classifier

# 使用示例
if __name__ == "__main__":
    filename = "./log/test_env_IO_log"
    model = train_random_forest(filename)