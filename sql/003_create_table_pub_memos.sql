DROP TABLE IF EXISTS public_memos;

CREATE TABLE public_memos (
  id INT NOT NULL AUTO_INCREMENT,
  memo INT DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO public_memos (memo) SELECT id FROM memos WHERE is_private=0 ORDER BY created_at ASC, id ASC;
