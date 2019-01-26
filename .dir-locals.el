
((nil . ((eval .
               (progn
                 ;; Reload code in `example` application
                 (defun reload-code-in-app ()
                   (interactive)
                   (async-shell-command "kill -SIGUSR1 $(pgrep example)")
                   )

                 ;; Do not show that annoying popup when shell command is finished
                 (add-to-list 'display-buffer-alist
                              (cons "\\*Async Shell Command\\*.*" (cons #'display-buffer-no-window nil)))

                 (global-set-key (kbd "C-c c r") 'reload-code-in-app)
                 )))))

