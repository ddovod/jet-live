
((nil . ((eval . (progn
                   (defun reload-code-in-app ()
                     (interactive)
                     (progn
                       (save-some-buffers)
                       (async-shell-command "pkill -SIGUSR1 example")))
                   (global-set-key (kbd "C-c c r") 'reload-code-in-app))))))

