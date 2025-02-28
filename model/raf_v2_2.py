import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
import joblib
from sklearn.tree import export_text

# 加载数据
def load_data(filename, num_samples=None):
    print("loading data...")
    X = []  # 输入特征
    y = []  # 输出标签

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
    return np.array(X), np.array(y)

# 保存特征重要性
def save_feature_importances(model, filename):
    importances = model.feature_importances_
    with open(filename, 'w') as f:
        for i, imp in enumerate(importances):
            f.write(f"Feature {i}: {imp}\n")
    print(f"Feature importances saved to {filename}")

# 保存每一颗树的决策规则
def save_tree_rules(model, filename):
    with open(filename, 'w') as f:
        for i, tree in enumerate(model.estimators_):
            f.write(f"Tree {i}:\n")
            tree_rules = export_text(tree, feature_names=[f"feature_{j}" for j in range(model.n_features_in_)])
            f.write(tree_rules)
            f.write("\n\n")
    print(f"Tree rules saved to {filename}")

# 训练随机森林
def train_random_forest(filename, num_samples=None):
    # 加载数据
    X, y = load_data(filename, num_samples)

    # 将数据拆分为训练集和测试集
    X_train, X_test, y_train, y_test = train_test_split(
        X,
        y,
        test_size=0.2,  # 20% 用于测试
        random_state=42,  # 固定随机种子以确保可重复性
    )

    # 创建并配置随机森林分类器
    rf_classifier = RandomForestClassifier(
        n_estimators=100,  # 决策树数量
        max_depth=10,  # 树的最大深度
        min_samples_split=2,  # 节点分裂所需的最小样本数
        min_samples_leaf=1,  # 叶子节点所需的最小样本数
        criterion="gini",  # 分裂质量的评估标准
        random_state=42,  # 随机种子
    )

    # 训练模型
    rf_classifier.fit(X_train, y_train)

    # 测试模型
    train_score = rf_classifier.score(X_train, y_train)
    test_score = rf_classifier.score(X_test, y_test)

    print(f"training set accuracy: {train_score:.4f}")
    print(f"test set accuracy: {test_score:.4f}")

    # 显示测试集中 5 个样本的预测结果
    print("\nprediction results of 5 samples in test set:")
    num_samples_to_show = min(5, len(X_test))  # 确保不超过测试集大小
    random_indices = np.random.choice(len(X_test), num_samples_to_show, replace=False)

    predictions = rf_classifier.predict(X_test[random_indices])

    for i, idx in enumerate(random_indices):
        print(f"\nsample {i + 1}:")
        print(f"input features: {X_test[idx]}")
        print(f"true label: {y_test[idx]}")
        print(f"predicted label: {predictions[i]}")
        print(f"prediction is correct: {predictions[i] == y_test[idx]}")

    # 保存模型
    model_filename = 'random_forest_model_3.joblib'
    joblib.dump(rf_classifier, model_filename)
    print(f"Model saved to {model_filename}")

    # 保存特征重要性
    feature_importances_filename = 'feature_importances_3.txt'
    save_feature_importances(rf_classifier, feature_importances_filename)

    # 保存树决策规则
    tree_rules_filename = 'tree_rules_3.txt'
    save_tree_rules(rf_classifier, tree_rules_filename)

    return rf_classifier

# 示例用法
if __name__ == "__main__":
    filename = "../log/test_env_IO_log"
    # model = train_random_forest(filename, num_samples=1000)  # 例如，只加载 1000 个样本
    model = train_random_forest(filename)  # 例如，只加载 1000 个样本