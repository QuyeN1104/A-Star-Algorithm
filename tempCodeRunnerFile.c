// Nếu chưa đủ 65% diện tích, thêm các vật cản đường thẳng không chồng lên nhau
    int total_cells = height * width;
    int obstacle_cells = 0;
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            if (map[i][j] == '#') obstacle_cells++;
    int max_attempts = 1000; // Giới hạn số lần thử để tránh chạy vô hạn
    int attempts = 0;
    while ((double)obstacle_cells / total_cells < 0.65 && attempts < max_attempts) {
        if (generate_random_obstacle(map, height, width, drone_size)) {
            obstacle_cells = 0; // Tính lại số ô bị chiếm bởi vật cản
            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                    if (map[i][j] == '#') obstacle_cells++;
        }
        attempts++;
    }