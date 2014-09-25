UPDATE memos SET title = substring_index(content, "\n",1);
