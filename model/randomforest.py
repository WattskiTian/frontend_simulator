import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split


# load data from file
def load_data(filename):
    print("loading data...")
    X = []  # input features
    y = []  # output labels

    with open(filename, "r") as file:
        lines = file.readlines()
        for i in range(0, len(lines) - 1, 2):  # process two lines at a time
            # process the first line (features)
            features = [int(x) for x in lines[i].strip().split()]
            X.append(features)
            # process the second line (label)
            label = int(lines[i + 1].strip())
            y.append(label)
    # print shape of X and y
    print(np.array(X).shape)
    print(np.array(y).shape)
    return np.array(X), np.array(y)


# train random forest
def train_random_forest(filename):
    # load data
    X, y = load_data(filename)

    # split data into training set and test set
    X_train, X_test, y_train, y_test = train_test_split(
        X,
        y,
        test_size=0.2,  # 20% of data for test
        random_state=42,  # fixed random seed for reproducibility
    )

    # create and configure random forest classifier
    rf_classifier = RandomForestClassifier(
        n_estimators=100,  # number of decision trees
        max_depth=10,  # max depth of trees
        min_samples_split=2,  # min samples to split a node
        min_samples_leaf=1,  # min samples to be a leaf node
        criterion="gini",  # quality of split
        random_state=42,  # random seed
    )

    # train model
    rf_classifier.fit(X_train, y_train)

    # test model
    train_score = rf_classifier.score(X_train, y_train)
    test_score = rf_classifier.score(X_test, y_test)

    print(f"training set accuracy: {train_score:.4f}")
    print(f"test set accuracy: {test_score:.4f}")

    # show prediction results of 5 samples in test set
    print("\nprediction results of 5 samples in test set:")
    # random select 5 indices
    num_samples = min(5, len(X_test))  # ensure not exceed the size of test set
    random_indices = np.random.choice(len(X_test), num_samples, replace=False)

    # get prediction results
    predictions = rf_classifier.predict(X_test[random_indices])

    # show results
    for i, idx in enumerate(random_indices):
        print(f"\nsample {i + 1}:")
        print(f"input features: {X_test[idx]}")
        print(f"true label: {y_test[idx]}")
        print(f"predicted label: {predictions[i]}")
        print(f"prediction is correct: {predictions[i] == y_test[idx]}")

    return rf_classifier


# example usage
if __name__ == "__main__":
    filename = "../log/test_env_IO_log"
    model = train_random_forest(filename)

