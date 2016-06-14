#pragma once
#include<numeric>
#include<vector>

class Layout { };

class VerticalLayout : public Layout {
public:
	void clear() {
		heights.clear();
	}

	void push(const int height) {
		heights.push_back(height);
	}

	double top(const int i) const {
		const int specified = std::accumulate(heights.begin(), heights.begin()+i, 0); 
		const int all = std::accumulate(heights.begin()+i, heights.end(), specified);
		return 1.*specified/all;
	}

	double bottom(const int i) const {
		const int specified = std::accumulate(heights.begin(), heights.begin()+i+1, 0); 
		const int all = std::accumulate(heights.begin()+i+1, heights.end(), specified);
		return 1.*specified/all;
	}

private:
	std::vector<int> heights;
};

